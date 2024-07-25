import ts.project
import io, cmd

class controller(cmd.Cmd):

  def __init__(self, db):
    super(controller, self).__init__()
    self.db = db
    self.project = None
    self.prompt = "() -> "
  
  def entries_str(self):
    with io.StringIO() as s:
      total = 0.0
      s.write("List of Timesheet Entries:\n")
      for (k, v) in self.db.items():
        s.write("\t{0}\t\t{1} Hours\n".format(k, round(v.total_hours, 2)))
        total += v.total_hours
      s.write("\t{0}\t\t{1}\n".format("-----", "---------"))
      s.write("\t{0}\t\t{1} Hours\n".format("TOTAL", round(total, 2)))
      return s.getvalue()
    
  def do_quit(self, line):
    return True
  
  def do_newsheet(self, line):
    name = line.strip().split()[0]
    if name in self.db:
      print("Project Already Exists")
      return
    e = ts.project.project()
    self.db[name] = e
    self.project = e
    self.prompt = "({0}) -> ".format(name)

  def do_ls(self, line):
    print(self.entries_str())

  def do_use(self, line):
    name = line.strip().split()[0]
    if name not in self.db:
      print("No Such Project")
      return
    self.project = self.db[name]
    self.prompt = "({0}) -> ".format(name)

  def complete_use(self, text, line, begidx, endidx):
    results = []
    for (k, v) in self.db.items():
      if k.startswith(text):
        results.append(k)
    return results
  
  def do_view(self, line):
    if self.project is None:
      print("No Project Selected")
      return
    print(self.project.__str__())

  def do_add(self, line):
    if self.project is None:
      print("No Project Selected")
      return
    num_hours = float(line.strip().split()[0])
    self.project.add_time(num_hours)

  def do_sub(self, line):
    if self.project is None:
      print("No Project Selected")
      return
    num_hours = float(line.strip().split()[0])
    self.project.sub_time(num_hours)

  def do_start(self, line):
    if self.project is None:
      print("No Project Selected")
      return
    self.project.start_period()
    print("Starting Time Period at {0}".format(self.project.period_start))

  def do_end(self, line):
    if self.project is None:
      print("No Project Selected")
      return
    old_hours = self.project.total_hours
    if line.strip().lower() == "noround":
      self.project.end_period(False)
    else:
      self.project.end_period()
    print("Added {0} Hours".format(self.project.total_hours - old_hours))
 