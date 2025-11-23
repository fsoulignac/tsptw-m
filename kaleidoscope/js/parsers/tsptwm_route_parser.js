class TSPTWRouteParser extends Parser
{
  get_attributes(obj)
  {
    return [];
  }

  detail_view_rows(obj, view_section) {
    view_section.add_label_row("Departure", this.f2(obj.departure));
    view_section.add_label_row("Arrival", this.f2(obj.arrival));
    view_section.add_label_row("Duration", this.f2(obj.duration));
    view_section.add_label_row("Path", this.jsonify(obj.path));
  }
}
kd.add_parser("tsptwm_route", new TSPTWRouteParser());
