from mininet.topo import Topo
from mininet.net import Mininet
from mininet.cli import CLI
from mininet.log import setLogLevel, info


class LeafSpineTopo(Topo):
    """
    Custom scalable Leaf-Spine topology.

    Parameters:
        k (int): switch radix (must be even)
                 - # spine switches = k / 2
                 - # leaf  switches = k
                 - # hosts per leaf = k / 2

    For example, for k = 4:
        - spine switches: 2  (s1, s2)
        - leaf switches : 4  (l1, l2, l3, l4)
        - hosts per leaf: 2  (total 8 hosts: h1..h8)
    """

    def build(self, k=4):
        # Ensure k is even so that the math above makes sense
        if k % 2 != 0:
            raise Exception("k (switch radix) must be even")

        num_spines = k // 2
        num_leaves = k
        hosts_per_leaf = k // 2

        info("*** Building Leaf-Spine topology with k=%d\n" % k)
        info("*** Spines: %d, Leaves: %d, Hosts/Leaf: %d\n"
             % (num_spines, num_leaves, hosts_per_leaf))

        # ------------------------
        # Create spine switches
        # ------------------------
        spines = []
        for i in range(1, num_spines + 1):
            spine = self.addSwitch("s%s" % i)    # s1, s2, ...
            spines.append(spine)

        # ------------------------
        # Create leaf switches
        # ------------------------
        leaves = []
        for j in range(1, num_leaves + 1):
            leaf = self.addSwitch("l%s" % j)     # l1, l2, ...
            leaves.append(leaf)

        # ------------------------
        # Attach hosts to each leaf
        # ------------------------
        host_id = 1
        for leaf in leaves:
            for _ in range(hosts_per_leaf):
                host = self.addHost("h%s" % host_id)
                self.addLink(host, leaf)         # host <-> leaf
                host_id += 1

        # ------------------------
        # Connect every leaf to every spine
        # (full mesh between leaf and spine layers)
        # ------------------------
        for spine in spines:
            for leaf in leaves:
                self.addLink(spine, leaf)        # spine <-> leaf


# This dictionary allows using the topology with:
#   sudo mn --custom assignment_14.py --topo leafspine,k=4
topos = {
    "leafspine": (lambda k=4: LeafSpineTopo(k=k))
}


if __name__ == "__main__":
    """
    Standalone mode:
        sudo python3 assignment_14.py

    This starts Mininet with our Leaf-Spine topology and no controller.
    You can then inspect the topology using:
        mininet> nodes
        mininet> links
        mininet> net

    For full connectivity (pingall), run an external controller
    (e.g., POX/Ryu) and start Mininet with --controller remote.
    """
    setLogLevel("info")

    # You can change k here to test scaling: 4, 6, 8, ...
    k = 4
    topo = LeafSpineTopo(k=k)

    # No internal controller here; suitable for topology inspection.
    net = Mininet(topo=topo,
                  controller=None,
                  autoSetMacs=True,
                  autoStaticArp=True)

    net.start()
    info("\n*** Network started. Use 'nodes', 'links', 'net' in the CLI to inspect.\n")
    CLI(net)
    net.stop()
