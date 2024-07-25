import sys,os
sys.path.append(os.getcwd())

import ts.controller
import os.path, sys, pickle

def main(argv=None):
  if argv is None:
    argv = sys.argv
  if len(argv) < 2:
    db_filename = os.path.expanduser("./.timesheetdb")
  else:
    db_filename = argv[1]
  if os.path.exists(db_filename):
    with open(db_filename, 'rb') as db_file:
      db = pickle.load(db_file)
  else:
    db = {}
    resp = None
    while resp is None:
      resp = input("Okay to Create New DB at {0} (Y/n)? ".format(db_filename))
      if len(resp) == 0:
        resp = 'y'
      resp = resp.strip().lower()
      if resp != 'y' and resp != 'n':
        resp = None
    if resp == 'n':
      print("No DB, aborting")
      sys.exit(1)
  cc = ts.controller.controller(db)
  cc.cmdloop()
  print("Writing Database")
  with open(db_filename, 'wb') as db_file:
    db = pickle.dump(db, db_file)

if __name__ == "__main__":
  main()
