import argparse, glob, logging, os, pickle, re, sys

def get_srl_package_mapping(directory):
    domains = {}
    logging.debug("get SRL domain <--> package mapping from directory '%s'" % directory)
    srl_files = glob.glob(os.path.join(directory,"atlas-srl-*"))
    for f in srl_files:
        logging.debug("reading file '%s'" % f)
        m =re.match("atlas-srl-(\w+)",os.path.basename(f))
        d = m.group(1)
        logging.info("extracted domain '%s' from filename '%s'" % (d,os.path.basename(f)))
        fh = open(f)
        pkgs = [p.strip() for p in fh.readlines() if p.strip()]
        fh.close()
        logging.debug("found packages %s in file '%s'" % (str(pkgs),f))
        if not d in domains:
            domains[d] = set(pkgs)
        else:
            domains[d].update(pkgs)

    for d,p in domains.items():
        logging.debug("domain '%s': packages = %s" % (d,str(p)))

    return domains

def get_srl_group_members(group_def_file):
    experts = {}
    logging.debug("read SRL group definitions from '%s'" % group_def_file)
    fh = open(group_def_file)
    groups = [l.strip() for l in fh.readlines()]
    fh.close()
    for g in groups:
        m = re.match("atlas-srl-(\w+).*=([\w ,]+)",g)
        domain = m.group(1)
        members = set([e.strip() for e in m.group(2).split(',')])
        logging.info("group '%s' consists of members %s" % (domain,members))
        if not domain in experts:
            experts[domain] = members
        else:
            experts[domain].update(members)

    for d,e in experts.items():
        logging.debug("domain '%s': experts = %s" % (d,str(e)))

    return experts

def merge_srl_packages_experts(pkg_map,expert_map):
    domains = {}
    for d,pkgs in pkg_map.items():
        if d in expert_map:
            domains[d] = {"packages": pkgs, "experts": expert_map[d]}
        else:
            logging.error("found package mapping for domain '%s' but no experts" % d)

    missing = set(expert_map.keys()) - set(domains.keys())
    if len(missing) > 0:
        logging.error("following domains have experts but no packages: %s" % str(missing))

    return domains

def main():
    parser = argparse.ArgumentParser(description="ATLAS SRL mapper",
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("indir",type=str,help="directory with ATLAS SRL definitions")
    parser.add_argument("outfile",type=str,help="output pickle file with dictionary")
    parser.add_argument("-v","--verbose",default="INFO",choices=["DEBUG","INFO","WARNING","ERROR","CRITICAL"],help="verbosity level")

    # get command line arguments
    args = parser.parse_args()
    args.indir = os.path.abspath(args.indir)

    # configure log output
    logging.basicConfig(format='%(asctime)s %(levelname)-10s %(message)s',
                        datefmt='%H:%M:%S',
                        level=args.verbose)

    # delegate
    packages = get_srl_package_mapping(os.path.join(args.indir,"done"))
    experts  = get_srl_group_members(os.path.join(args.indir,"atlas-srl_group_defs.txt"))
    domains  = merge_srl_packages_experts(packages,experts)

    if os.path.isfile(args.outfile):
        var = ""
        while not (var == 'y' or var == 'n'):
            var = raw_input("The output file '%s' already exists. Do you want to overwrite it? y|[n] " % args.outfile)
        if var == 'n':
            sys.exit(0)

    pickle.dump(domains,open(args.outfile,'w'))

if __name__ == "__main__":
    main()
