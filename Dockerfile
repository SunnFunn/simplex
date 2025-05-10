FROM apache/airflow:3.0.0
COPY requirements.txt /
RUN pip install --no-cache-dir "apache-airflow==${AIRFLOW_VERSION}" -r /requirements.txt

USER root
RUN apt-get update -y && apt-get install -y build-essential && apt-get install -y make
USER airflow

RUN mkdir /opt/airflow/app
COPY ./app/ /opt/airflow/app/