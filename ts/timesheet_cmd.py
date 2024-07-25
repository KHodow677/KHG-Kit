import ts.time_entry
import io, cmd

class timesheet_cmd(cmd.Cmd):

  def __init__(self, db):
    super(timesheet_cmd, self).__init__()
    self.db = db
    self.entry = None
    self.prompt = "> "
  
  def entries_str(self):
    with io.StringIO() as s:
      s.write("List of Timesheet Entries:\n")
      for (k, v) in self.db.items():
        s.write("\t{0}\n".format(k))
      return s.getvalue()
    
  def do_quit(self, line):
    return True
  
  def do_newsheet(self, line):
    name = line.strip().split()[0]
    if name in self.db:
      print("Entry Already Exists")
      return
    e = ts.time_entry.time_entry()
    self.db[name] = e
    self.entry = e
    self.prompt = "({0}) > ".format(name)

  def do_ls(self, line):
    print(self.entries_str())

  def do_use(self, line):
    name = line.strip().split()[0]
    if name not in self.db:
      print("No Such Entry")
      return
    self.entry = self.db[name]
    self.prompt = "({0}) > ".format(name)

  def complete_use(self, text, line, begidx, endidx):
    results = []
    for (k, v) in self.db.items():
      if k.startswith(text):
        results.append(k)
    return results
  
  def do_view(self, line):
    if self.entry is None:
      print("No Entry Selected")
      return
    print(self.entry.__str__())

  def do_add(self, line):
    if self.entry is None:
      print("No Entry Selected")
      return
    num_hours = float(line.strip().split()[0])
    self.entry.add_time(num_hours)

  def do_unadd(self, line):
    if self.entry is None:
      print("No Entry Selected")
      return
    num_hours = float(line.strip().split()[0])
    self.entry.unadd_time(num_hours)

  def do_start(self, line):
    if self.entry is None:
      print("No Entry Selected")
      return
    self.entry.start_period()
    print("Starting Time Period at {0}".format(self.entry.period_start))

  def do_end(self, line):
    if self.entry is None:
      print("No Entry Selected")
      return
    old_hours = self.entry.total_hours
    if line.strip().lower() == 'noround':
      self.entry.end_period(False)
    else:
      self.entry.end_period()
    print("Added {0} Hours".format(self.entry.total_hours - old_hours))
 