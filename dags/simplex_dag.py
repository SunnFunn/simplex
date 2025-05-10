import sys, os
import subprocess as sp
import pendulum

# adding paths to python modules to enable import
current = os.path.dirname(os.path.realpath(__file__))
parent = os.path.dirname(current)
sys.path.append(parent)

from app import data


from airflow.sdk import dag, task
@dag(
    schedule=None,
    start_date=pendulum.datetime(2025, 5, 10, tz="UTC"),
    catchup=False,
    tags=["example"],
)
def simplex_taskflow_api():
    @task()
    def build():
        args1 = ['rm', f'{parent}/app/build/opz']
        args2 = ['make', '-C', f'{parent}/app/']
        sp.run(args1, text=True)
        result = sp.run(args2, capture_output=True, text=True)
        if not result.stderr:
            return result.stdout
        else:
            return result.stderr
    
    @task()
    def retrive_raw_data():
        input_data = dict()
        input_data.update({"vars_number": 6})
        input_data.update({"constraints_number": 5})
        input_data.update({"demand": (19,15,12,11,15)})
        input_data.update({"costs": (50,40,25,60,35,30)})
        input_data.update({"constraints_coefficients": (1,1,1,0,0,0,
                                                        0,0,0,1,1,1,
                                                        1,0,0,1,0,0,
                                                        0,1,0,0,1,0,
                                                        0,0,1,0,0,1)})
        # inequalities_number = 5
        # inequalities_type = {"neg", "neg", "pos", "pos", "pos"}
        return input_data
    
    @task()
    def struct_input(input_data):
        data.pack(input_data)
        return True
    
    @task()
    def optimize(data_status):
        if data_status:
            args = [parent + '/app/build/opz', parent + '/app/data/data.bin']
            result = sp.run(args, capture_output=True, text=True)
            if not result.stderr:
                return result.stdout
            else:
                return result.stderr
        else:
            return "Data for optimization failed to be prepared!"
    
    build()
    input_data = retrive_raw_data()
    data_status = struct_input(input_data)
    optimize(data_status=data_status)

simplex_taskflow_api()