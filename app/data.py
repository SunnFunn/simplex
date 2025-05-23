import redis


data_dict = {"supply": (20,15),
            "demand": (30,4,1),
            "costs": (7,8,9,1,5,12)}

supply_dict = {"s1": 20, "s2": 15}
demand_dict = {"d1": 30, "d2": 4, "d3": 1}

pbVars = ""
for key_s in supply_dict.keys():
    for key_d in demand_dict.keys():
        pbVars += f"{key_s}_{key_d},"




# def data_to_db(data, host, port, db, password, extime):
#     supply_str = ""
#     for s in data["supply"]:
#         supply_str += f"{s},"
#     demand_str = ""
#     for d in data["demand"]:
#         demand_str += f"{d},"
#     costs_str = ""
#     for c in data["costs"]:
#         costs_str += f"{c},"
    
#     r = redis.Redis(host=host, port=port, db=db, password=password)

#     r.setex("supply_str", extime, supply_str[:-1])
#     r.setex("demand_str", extime, demand_str[:-1])
#     r.setex("costs_str", extime, costs_str[:-1])


# def data_from_db(host, port, db, password, extime):
#     r = redis.Redis(host=host, port=port, db=db, password=password)

#     supply_str = r.get("supply_str")
#     demand_str = r.get("demand_str")
#     costs_str = r.get("costs_str")

#     print(supply_str)
#     print(demand_str)
#     print(costs_str)


if __name__ == "__main__":
    # data_to_db(data_dict, host="0.0.0.0", port=6380, db=0, password="alext", extime=600)
    # data_from_db(host="0.0.0.0", port=6380, db=0, password="alext", extime=600)
    print(pbVars[:-1])