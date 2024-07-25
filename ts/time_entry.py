import datetime

class time_entry(object):

  def __init__(self):
    self.total_hours = 0.0
    self.curr_hours = 0.0
    self.period_start = None

  def __str__(self):
    s = "Current: {0} hours\nTotal: {1} hours".format(self.curr_hours, self.total_hours)
    if self.period_start is not None:
      s += "\nPeriod started: {0}".format(self.period_start)
    return s
  
  def add_time(self, num_hours):
    self.total_hours += num_hours
    self.curr_hours += num_hours

  def sub_time(self, num_hours):
    self.curr_hours -= num_hours

  def start_period(self):
    self.period_start = datetime.datetime.now()

  def end_period(self, round_to_qtrhour = True):
    period_end = datetime.datetime.now()
    td = period_end - self.period_start
    nh = td.total_seconds() / (60 * 60)
    if (round_to_qtrhour):
      nh = round(nh * 4)
    self.add_time(nh)
    self.period_start = None
