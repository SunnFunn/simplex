import sys, os
import subprocess as sp
import pendulum

# adding paths to python modules to enable import
current = os.path.dirname(os.path.realpath(__file__))
parent = os.path.dirname(current)
sys.path.append(parent)

from app import data


host = "0.0.0.0"
port=6380
db=0
password="alext"
extime=1200

from airflow.sdk import dag, task
@dag(
    schedule=None,
    start_date=pendulum.datetime(2025, 5, 11, tz="UTC"),
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
        data_dict = {"supply": (160,30,90),
                    "demand": (100,40,80,60),
                    "costs": (4,8,10,5,4,6,2,3,4,4,6,5)}
        return data_dict
    
    @task()
    def load_data(data_dict):
        data.data_to_db(data=data, host=host, port=port, db=db, password=password, extime=extime)
        return True
    
    @task()
    def optimize(data_status):
        if data_status:
            args = [parent + '/app/build/opz', host, port, password]
            result = sp.run(args, capture_output=True, text=True)
            if not result.stderr:
                return result.stdout
            else:
                return result.stderr
        else:
            return "Data for optimization failed to be prepared!"
    
    build()
    input_data = retrive_raw_data()
    data_status = load_data(input_data)
    optimize(data_status=data_status)

simplex_taskflow_api()