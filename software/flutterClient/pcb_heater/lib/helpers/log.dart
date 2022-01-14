import 'package:flutter/foundation.dart';

class Log {
  static write(String s) {
    if (kDebugMode) {
      print(s);
    }
  }
}
