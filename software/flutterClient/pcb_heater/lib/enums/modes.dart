class Mode {
  String name;
  int id;

  Mode(this.id, this.name);

  static List<Mode> values = <Mode>[
    Mode(0, "Off"),
    Mode(1, "Heat"),
    Mode(2, "Cold"),
  ];
}
