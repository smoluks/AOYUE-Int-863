import '/dto/device.dart';
import '/modbus/modbus.dart';

class ProtocolController {
  ModbusClient client;
  int? _channelsCount;

  ProtocolController(Device device)
      : client = createBluetoothClient(device.address);

  Future<void> connect() async {
    await client.connect();
  }

  void dispose() {
    client.close();
  }

  Future<DeviceState> getState() async {
    List<int> regs;

    if (_channelsCount == null) {
      var result = await client.readInputRegisters(0, 20);
      regs = result.toList();
      _channelsCount = regs.removeAt(0);
    } else {
      var result = await client.readInputRegisters(1, _channelsCount! * 2 + 3);
      regs = result.toList();
    }

    int error = regs.removeAt(0);
    int btError = regs.removeAt(0);
    regs.removeAt(0);

    var states = <SensorState>[];
    for (int i = 0; i < _channelsCount!; i++) {
      states.add(SensorState(regs.removeAt(0), regs.removeAt(0)));
    }

    return DeviceState(error, btError, states);
  }

  Future<List<Target>> getTargets() async {
    _channelsCount ??= await getChannelsCount();

    var result = await client.readHoldingRegisters(1, _channelsCount!);

    return result.map((e) => Target(e)).toList();
  }

  setTarget(int sensorNumber, Target target) async {
    var result = _convertTo18b20Temperature(target.temperature);
    if (target.channel1) {
      result |= 0x4000;
    }
    if (target.channel2) {
      result |= 0x8000;
    }

    await client.writeSingleRegister(sensorNumber + 1, result);
  }

  Future<int> getMode() async {
    var result = await client.readHoldingRegisters(0, 1);
    return result[0];
  }

  setMode(int mode) async {
    await client.writeSingleRegister(0, mode);
  }

  Future<int> getChannelsCount() async {
    var result = await client.readInputRegisters(0, 1);
    _channelsCount = result[0];

    return _channelsCount!;
  }

  static double _convert18b20Temperature(int f) {
    f = f & 0x3FFF;
    if (f & 0x2000 == 0x2000) {
      //minus
      return (0x4000 - f) / 16;
    }

    return f / 16;
  }

  static int _convertTo18b20Temperature(double f) {
    return (f * 16).round() & 0x3FFF;
  }
}

class DeviceState {
  int error;
  int btError;
  List<SensorState> sensorStates;

  DeviceState(this.error, this.btError, this.sensorStates);
}

class SensorState {
  double temperature = 0;
  bool isPresent = false;
  int error = 0;

  SensorState(int valueReg, int errorReg) {
    temperature = ProtocolController._convert18b20Temperature(valueReg);
    isPresent = (valueReg & 0x8000) == 0x8000;
    error = errorReg;
  }
}

class Target {
  double temperature = 0;
  bool channel1 = false;
  bool channel2 = false;

  Target(int e) {
    temperature = ProtocolController._convert18b20Temperature(e);
    channel1 = (e & 0x4000 == 0x4000);
    channel2 = (e & 0x8000 == 0x8000);
  }

  Target.constructor2(this.temperature, this.channel1, this.channel2);
}
