class TSPTWMLBFSParser extends Parser
{
  get_attributes(obj)
  {
    return [
      new Attribute("Time", this.f2(obj.time)),
      new Attribute("Enumerated", this.f2(obj.enumerated_count)),
      new Attribute("Dominated", this.f2(obj.dominated_count)),
      new Attribute("Discarded", this.f2(obj.discarded_count)),
      new Attribute("Undominated", this.f2(obj.undominated_count)),
    ];
  }

  detail_view_rows(obj, view_section) {

    view_section.add_table_row(
      ["Time", "Enumerated", "Dominated", "Discarded", "Undominated"],
      [obj.time, obj.enumerated, obj.dominated, obj.discarded, obj.undominated],
    );

    const add_bar = (data, title, value) => {
      if (value && value !== 0) {
        data.x.push(title);
        data.y.push(value);
      }
    };

    // #Labels plot.
    var data_lbfs = {x:[], y:[], name:"LBFS"};
    add_bar(data_lbfs, "Enumerated", obj.enumerated_count);
    add_bar(data_lbfs, "Dominated", obj.dominated_count);
    add_bar(data_lbfs, "Discarded", obj.discarded_count);
    add_bar(data_lbfs, "Undominated", obj.undominated_count);
    const labels_plot = bar_plot("#Labels by stage", [data_lbfs], {height:300, width:300});

    view_section.add_flex_row([labels_plot]);
  }
}
kd.add_parser("tsptwm_lbfs", new TSPTWMLBFSParser());
