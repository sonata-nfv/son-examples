
#!/bin/python

import sys
import yaml
import os

NET_PATH = "/sys/class/net"

def collect_intf_statistics():
    r = dict()
    intf_list = os.listdir(NET_PATH)
    for intf_name in intf_list:
        print "----" + intf_name
        stat_files = os.listdir(os.path.join(NET_PATH, intf_name, "statistics"))
        for fn in stat_files:
            with open(os.path.join(NET_PATH, intf_name, "statistics", fn), "r") as f:
                data = -1
                try:
                    data = float(f.read())
                except:
                    pass
                r["stat__" + intf_name + "__" + fn] = data
    return r


def update_yml(path, data):
    old_data = dict()
    # try to load existing data
    if os.path.exists(path):
        print "Reading %r" % path
        with open(path, "r") as f:
            old_data = yaml.load(f)
    # inject new data
    old_data.update(data)
    # write yml
    print "Writing %r" % path
    with open(path, "w") as f:
        yaml.safe_dump(old_data, f, default_flow_style=False)

def main():
    OUTPUT = sys.argv[1]
    print "Collecting intf statistics..."
    print "Out: %s" % OUTPUT
    update_yml(OUTPUT, collect_intf_statistics())
    print "done."


if __name__ == '__main__':
    main()
