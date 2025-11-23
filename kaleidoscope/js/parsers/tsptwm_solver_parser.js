class TSPTWMSolverParser extends Parser
{

  get_attributes(obj)
  {
    return [
      new Attribute("Time", this.f2(obj.time)),
      new Attribute("Makespan Time", this.f2(obj.makespan_time)),
      new Attribute("Reverse MS Time", this.f2(obj.reverse_makespan_time)),
      new Attribute("Status", obj.status),
      new Attribute("Makespan Status", obj.makespan_status),
      new Attribute("Reverse MS Status", obj.reverse_makespan_status),
      new Attribute("Final UB", this.f2(obj.ub)),
      new Attribute("Makespan UB", this.f2(obj.ms_ub)),
      new Attribute("Reverse MS UB", this.f2(obj.reverse_ms_ub)),
      new Attribute("Preprocess Time", this.f2(obj.preprocess_time)),
      new Attribute("Enumeration Time", this.f2(obj.enum_time)),
      new Attribute("MS Preprocess Time", this.f2(obj.ms_preprocess_time)),
      new Attribute("MS Enumeration Time", this.f2(obj.ms_enum_time)),
    ];
  }

  detail_view_rows(obj, view_section) {

    view_section.add_table_row(
      ["Makespan Time", "Makespan Status", "Makespan UB"],
      [obj.makespan_time, obj.makespan_status, obj.ms_ub]
    );

    view_section.add_table_row(
      ["Reverse MS Time", "Reverse MS Status", "Reverse MS UB"],
      [obj.reverse_makespan_time, obj.reverse_makespan_status, obj.reverse_ms_ub]
    );
    
    view_section.add_table_row(
      ["Time", "Status", "Final UB"],
      [obj.time, obj.status, obj.ub]
    );

    view_section.add_table_row(
      ["Preprocess Time", "Enumeration Time", "MS Preprocess Time", "MS Enumeration Time"],
      [obj.preprocess_time, obj.enum_time, obj.ms_preprocess_time, obj.ms_enum_time]
    );

    if(obj.iterations.length > 0) {
        view_section.add_label_row("MS iterations", " ");
        view_section.add_slider_row(
        obj.iterations,
          (iter, ms, ms_section) => {
            ms_section.add_html_row(`<span class="slider_header">#${iter+1}</span>`);
            kd.get_parser(ms.kd_type)?.detail_view_rows(ms, ms_section);
          }
        );
    }

  }
}
kd.add_parser("tsptwm_solver", new TSPTWMSolverParser());
