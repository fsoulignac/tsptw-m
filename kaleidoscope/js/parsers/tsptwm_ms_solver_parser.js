class TSPTWMMSSolverParser extends Parser
{
 
  get_attributes(obj)
  {
    return [
      new Attribute("Time", this.f2(obj.time)),
      new Attribute("Status", obj.status),
      new Attribute("Final UB", this.f2(obj.ub)),
    ];
  }

  detail_view_rows(obj, view_section) {

    view_section.add_table_row(
      ["Time", "Status", "Final UB"],
      [obj.time, obj.status, obj.ub]
    );
        
    if(obj.lbfs_iterations.length > 0) {
        view_section.add_label_row("LBFS iterations", " ");
        view_section.add_slider_row(
        obj.lbfs_iterations,
          (iter, lbfs, lbfs_section) => {
            lbfs_section.add_html_row(`<span class="slider_header">#${iter+1}</span>`);
            kd.get_parser(lbfs.kd_type)?.detail_view_rows(lbfs, lbfs_section);
          }
        );
    }
  }
}
kd.add_parser("tsptwm_ms_solver", new TSPTWMMSSolverParser());
