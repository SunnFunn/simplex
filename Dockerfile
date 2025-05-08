FROM python:3.11

RUN apt-get -y update
# RUN apt-get install gcc
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

RUN groupadd -r simplex && useradd -r -g simplex simplex
RUN echo 'simplex:alext' | chpasswd

RUN mkdir app
COPY ./app/ ./app/
RUN chown -R simplex:root app

USER simplex

CMD ["python3", "/app/data.py"]