import shutil
import os

Import("env")

print("===== copying lv_conf.h to lib dir ===== ")

# Source and destination paths
SRC = os.path.join("include", "lv_conf.h")
DST_DIR = os.path.join(".pio", "libdeps", "QuantumClockHAX", "lvgl")
DST = os.path.join(DST_DIR, "lv_conf.h")

print(f"Source: {SRC}")
print(f"Destination: {DST}")

def copy_lv_conf():
    if not os.path.exists(SRC):
        print(f"Source file not found: {SRC}")
        return
    if not os.path.exists(DST_DIR):
        print(f"Destination directory not found: {DST_DIR}")
        return
    shutil.copy2(SRC, DST)    
    print(f"Copied {SRC} to {DST}")
    print("===== Done copying lv_conf.h to lib dir ===== ")    

copy_lv_conf()
