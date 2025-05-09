import json
import sys, os
import pendulum

# adding paths to python modules to enable import
current = os.path.dirname(os.path.realpath(__file__))
parent = os.path.dirname(current)
sys.path.append(parent)

from app import data


from airflow.sdk import dag, task
@dag(
    schedule=None,
    start_date=pendulum.datetime(2025, 5, 9, tz="UTC"),
    catchup=False,
    tags=["example"],
)
def simplex_taskflow_api():
    @task()
    def retrive_raw_data():
        input_data = dict()
        input_data.update({"vars_number": 6})
        input_data.update({"constraints_number": 5})
        input_data.update({"demand": (8,25,13,8,5)})
        input_data.update({"costs": (90,20,85,60,35,30)})
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
    
    input_data = retrive_raw_data()
    struct_input(input_data)

simplex_taskflow_api()