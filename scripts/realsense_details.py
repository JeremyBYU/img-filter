import pyrealsense2 as rs

def main():
    ctx = rs.context()
    sensors = ctx.query_all_sensors() 
    depth_sensor = sensors[0]
    depth_sensor = depth_sensor.as_depth_sensor() 
    depth_scale = depth_sensor.get_depth_scale() 
    depth_baseline = 50.0021
    profile = depth_sensor.profiles[77] 
#   425.849,

if __name__ == "__main__":
    main()