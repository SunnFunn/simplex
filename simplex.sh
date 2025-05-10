#!/bin/bash


# hash to check password
simplexhash="WtacgFOJGmCVEyHldn3yXFoxT4UemwZm8TK63bpzu5ht5nDKb2iVHBbkTs.bvU0lyfkIx6sp/sRkAiml.174M/"
# Set $ as the delimiter
IFS='$'

echo "Enter password to start redis container: "
# hide entering password
stty -echo
read -r pass
stty echo

# hash entered pass with the our salt
hashedpass=$(openssl passwd -6 --salt Sg378mtJ "$pass")

# hashed pass splitted by IFS delimiter so let's make these as arameters to use them in "if" statement
set -- $hashedpass

if [ "$4" == "$simplexhash" ];
then
    echo "Correct password entered."
else
    echo "No or wrong password entered!"
    exit
fi

# pass env vars to airflow docker
touch .env

echo "AIRFLOW_UID=501
USERNAME=$(whoami)
PASSWORD=$pass
DBNAME=simplex" >> .env

docker compose up airflow-init
# exCode=$?
# echo $exCode
# if [ $exCode -ne 0 ];
# then
#     echo "Airflow init failed!"
# fi

# docker compose up

# rm .env