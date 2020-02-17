import leviathans_breath
import yaml


class SerializableClass(object):
    INVALID_INDEX = -1

    def to_dict(self):
        return {attr: getattr(self, attr) for attr in vars(self)}

    @classmethod
    def from_dict(cls, data):
        ret = cls()
        for key in data:
            if hasattr(ret, key):
                setattr(ret, key, data[key])
        return ret


class FanConfig(SerializableClass):
    def __init__(self):
        self.index = SerializableClass.INVALID_INDEX
        self.name = ""
        self.temperature_source = SerializableClass.INVALID_INDEX
        self.pwm_controlled = False


class TemperatureConfig(SerializableClass):
    def __init__(self):
        self.index = SerializableClass.INVALID_INDEX
        self.name = ""


class LEDConfig(SerializableClass):
    SELF_SYNC_INDEX = -2

    def __init__(self):
        self.index = SerializableClass.INVALID_INDEX
        self.name = ""
        self.temperature_source = SerializableClass.INVALID_INDEX
        self.time_controlled = True
        self.channel_sync = [LEDConfig.SELF_SYNC_INDEX for _ in range(leviathans_breath.NUM_LED_CHANNELS())]


class FanCurvePreset(SerializableClass):
    def __init__(self):
        self.name = ""
        self.curve = []  # type: list[tuple[float, float]]


class LEDCurvePreset(SerializableClass):
    def __init__(self):
        self.name = ""
        self.curve = [[] for _ in range(leviathans_breath.NUM_LED_CHANNELS())]  # type: list[list[tuple[float, float]]]
        self.channel_sync = [LEDConfig.SELF_SYNC_INDEX for _ in range(leviathans_breath.NUM_LED_CHANNELS())]


class DB(object):
    FAN_CONFIGS_KEY = "fans"
    LED_CONFIGS_KEY = "leds"
    TEMPERATURE_CONFIGS_KEY = "temperatures"
    FAN_PRESET_CONFIGS_KEY = "fan_presets"
    LED_PRESET_CONFIGS_KEY = "led_presets"

    def __init__(self, file_name: str = None):
        self.file_name = file_name

        self.fan_configs = []  # type: list[FanConfig]
        self.led_configs = []  # type: list[LEDConfig]
        self.temperature_configs = []  # type: list[TemperatureConfig]
        self.fan_presets = []  # type: list[FanCurvePreset]
        self.led_presets = []  # type: list[LEDCurvePreset]

        self.fan_configs_name_map_ = dict()
        self.led_configs_name_map_ = dict()
        self.temperature_configs_name_map_ = dict()
        self.fan_presets_name_map_ = dict()
        self.led_presets_name_map_ = dict()

    def init_maps_(self):
        for name_map in [self.fan_configs_name_map_, self.led_configs_name_map_, self.temperature_configs_name_map_,
                         self.fan_presets_name_map_, self.led_presets_name_map_]:
            name_map.clear()

        self.fan_configs_name_map_ = {fan_config.name: fan_config for fan_config in self.fan_configs}
        self.led_configs_name_map_ = {led_config.name: led_config for led_config in self.led_configs}
        self.temperature_configs_name_map_ = {temperature_config.name: temperature_config for temperature_config
                                              in self.temperature_configs}
        self.fan_presets_name_map_ = {fan_preset.name: fan_preset for fan_preset in self.fan_presets}
        self.led_presets_name_map_ = {led_preset.name: led_preset for led_preset in self.led_presets}

    @staticmethod
    def build_default():
        ret = DB()
        for idx in range(leviathans_breath.NUM_FANS()):
            config = FanConfig()
            config.index = idx
            config.name = "FAN{}".format(idx + 1)
            config.temperature_source = 0
            ret.fan_configs.append(config)
        for idx in range(leviathans_breath.NUM_LEDS()):
            config = LEDConfig()
            config.index = idx
            config.name = "LED{}".format(idx + 1)
            config.temperature_source = 0
            ret.led_configs.append(config)
        for idx in range(leviathans_breath.NUM_TEMPERATURE_SENSORS()):
            config = TemperatureConfig()
            config.index = 0
            config.name = "TEMP_IN{}".format(idx + 1)
            ret.temperature_configs.append(config)

        return ret

    def to_dict(self):
        store = dict()

        store[DB.FAN_CONFIGS_KEY] = [fan_config.to_dict() for fan_config in self.fan_configs]
        store[DB.LED_CONFIGS_KEY] = [led_config.to_dict() for led_config in self.led_configs]
        store[DB.TEMPERATURE_CONFIGS_KEY] = [temp_config.to_dict() for temp_config in self.temperature_configs]
        if self.fan_presets:
            store[DB.FAN_PRESET_CONFIGS_KEY] = [fan_preset.to_dict() for fan_preset in self.fan_presets]
        if self.led_presets:
            store[DB.LED_PRESET_CONFIGS_KEY] = [led_preset.to_dict() for led_preset in self.led_presets]

        return store

    def save(self, file_name=None):
        if file_name:
            self.file_name = file_name

        store = self.to_dict()

        with open(self.file_name, 'w') as file:
            yaml.dump(store, file)

    def load(self, file_name=None):
        if file_name:
            self.file_name = file_name

        with open(self.file_name, 'r') as file:
            store = yaml.full_load(file)

        for configs in [self.fan_configs, self.led_configs, self.temperature_configs]:
            configs.clear()

        if DB.FAN_PRESET_CONFIGS_KEY in store:
            self.fan_presets.clear()
        if DB.LED_PRESET_CONFIGS_KEY in store:
            self.led_presets.clear()

        for configs_key, configs, config_class in zip(
                [DB.FAN_CONFIGS_KEY, DB.LED_CONFIGS_KEY, DB.TEMPERATURE_CONFIGS_KEY],
                [self.fan_configs, self.led_configs, self.temperature_configs],
                [FanConfig, LEDConfig, TemperatureConfig]):
            for config in sorted(store[configs_key], key=lambda elem: elem['index']):
                configs.append(config_class.from_dict(config))

        if DB.FAN_PRESET_CONFIGS_KEY in store:
            for preset in store[DB.FAN_PRESET_CONFIGS_KEY]:
                self.fan_presets.append(FanCurvePreset.from_dict(preset))
        if DB.LED_PRESET_CONFIGS_KEY in store:
            for preset in store[DB.LED_PRESET_CONFIGS_KEY]:
                self.led_presets.append(LEDCurvePreset.from_dict(preset))

    def __str__(self):
        return yaml.dump(self.to_dict())

    def get_fan_config(self, name):
        return self.fan_configs_name_map_[name]

    def get_led_config(self, name):
        return self.led_configs_name_map_[name]

    def get_temperature_config(self, name):
        return self.temperature_configs[name]

    def get_fan_preset(self, name):
        return self.fan_presets_name_map_[name]

    def get_led_preset(self, name):
        return self.led_presets_name_map_[name]


def main():
    db = DB.build_default()
    db.save("db.yaml")
    db.load()

    other = DB.build_default()
    assert str(db) == str(other)


if __name__ == '__main__':
    main()
