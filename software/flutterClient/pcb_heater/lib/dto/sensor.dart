import 'package:charts_flutter/flutter.dart' as charts;
import 'package:pcb_heater/helpers/protocol_controller.dart';

class TimestampedValue {
  late DateTime timestamp;
  double value;

  TimestampedValue(this.value) {
    timestamp = DateTime.now();
  }
}

class Sensor {
  int sensorNumber;
  double currentTemperature = 0;
  double targetTemperature = 0;
  bool isPresent = false;
  int error = 0;

  bool channel1 = false;
  bool channel2 = false;

  late charts.Series<TimestampedValue, DateTime> seriesList;
  List<TimestampedValue> data = [];

  Sensor(this.sensorNumber) {
    seriesList = charts.Series<TimestampedValue, DateTime>(
      id: "ch$sensorNumber",
      colorFn: (_, __) => charts.MaterialPalette.blue.shadeDefault,
      domainFn: (TimestampedValue value, int? n) => value.timestamp,
      measureFn: (TimestampedValue value, int? n) => value.value,
      measureLowerBoundFn: (TimestampedValue value, _) => 0,
      measureUpperBoundFn: (TimestampedValue value, _) => targetTemperature,
      data: data,
    );
  }

  void setState(SensorState sensorState) {
    error = sensorState.error;
    isPresent = sensorState.isPresent;
    currentTemperature = sensorState.temperature;

    if (error == 0 && isPresent) {
      currentTemperature = sensorState.temperature;
      data.add(TimestampedValue(currentTemperature));
    }
  }
}
