import multiprocessing
import os
import queue

STOP_TIMEOUT = 0.4
CLOSE_MESSAGE = "close"
KILL_ME_MESSAGE = "kill me"

driver_process_ = None  # type: multiprocessing.Process
driver_mosi_queue_ = multiprocessing.Queue()
driver_miso_queue_ = multiprocessing.Queue()


def generic_call_(func_args, default_return):
    if not ensure_driver_master_():
        raise RuntimeError("Unable to connect to Leviathan's Breath")

    driver_mosi_queue_.put(func_args)
    ret = driver_miso_queue_.get(timeout=STOP_TIMEOUT)

    if ret[0] == KILL_ME_MESSAGE:
        return default_return
    return ret[0]


def connect():
    return generic_call_(["connect"], False)


def is_connected():
    return generic_call_(["is_connected"], False)


def sync():
    return generic_call_(["sync"], False)


def get_fan_curve(index):
    return generic_call_(["get_fan_curve", index], None)


def send_fan_curve(index, curve):
    return generic_call_(["send_fan_curve", index, curve], False)


def get_led_curve(index):
    return generic_call_(["get_led_curve", index], None)


def send_led_curve(index, curve):
    return generic_call_(["send_led_curve", index, curve], False)


def get_all_fan_rpms():
    return generic_call_(["get_all_fan_rpms"], None)


def get_all_temperatures():
    return generic_call_(["get_all_temperatures"], None)


def get_all_fan_parameters():
    return generic_call_(["get_all_fan_parameters"], None)


def set_fan_parameters(index, pwm_controlled, temperature_sensor_idx):
    return generic_call_(["set_fan_parameters", index, pwm_controlled, temperature_sensor_idx], False)


def get_all_led_parameters():
    return generic_call_(["get_all_led_parameters"], None)


def set_led_parameters(index, time_controlled, speed_multiplier, temperature_sensor_idx):
    return generic_call_(["set_led_parameters", index, time_controlled, speed_multiplier, temperature_sensor_idx], None)


def start_driver_process():
    global driver_process_

    driver_process_ = multiprocessing.Process(target=driver_loop_, args=(driver_mosi_queue_, driver_miso_queue_))
    driver_process_.start()


def stop_driver_process():
    global driver_process_

    driver_mosi_queue_.put(["close"])
    driver_process_.join(STOP_TIMEOUT)
    if driver_process_.is_alive():
        driver_process_.kill()


def driver_loop_(mosi_queue: multiprocessing.Queue, miso_queue: multiprocessing.Queue):
    import leviathans_breath

    driver = leviathans_breath.Driver()
    driver.connect()

    while True:
        ppid = os.getppid()
        try:
            command = mosi_queue.get(timeout=STOP_TIMEOUT)
        except queue.Empty:
            if ppid == 1:
                return
            else:
                continue

        if len(command) == 1 and command[0] == CLOSE_MESSAGE:
            return

        if not ensure_driver_slave_(driver, miso_queue):
            return

        func = getattr(driver, command[0])
        ret = func(*command[1:])

        miso_queue.put([ret])


def ensure_driver_slave_(driver, miso_queue: multiprocessing.Queue):
    if not driver.sync():
        miso_queue.put([KILL_ME_MESSAGE])
        return False
    return True


def ensure_driver_master_(retry_count=3):
    if retry_count == 0:
        return False

    driver_mosi_queue_.put(["sync"])
    try:
        ret = driver_miso_queue_.get(timeout=STOP_TIMEOUT)
    except queue.Empty:
        ret = [KILL_ME_MESSAGE]

    if ret[0] == KILL_ME_MESSAGE:
        stop_driver_process()
        start_driver_process()

    if ret[0] is True:
        return True

    return ensure_driver_master_(retry_count - 1)
