#
# $Id: DXphipi.py,v 1.1 2007-03-01 17:15:46 msmizans Exp $
#
PythiaB = Algorithm( "PythiaB" )

# For Ds- -> phipi-: open Ds channels only for particle Ds+
if DXChannel == "Dsphipi":
    PythiaB.PythiaCommand += [  "pydat3 mdme 818 1 2"
           ,"pydat3 mdme 819 1 2"
           ,"pydat3 mdme 820 1 2"
           ,"pydat3 mdme 821 1 2"
           ,"pydat3 mdme 822 1 2"
           ,"pydat3 mdme 823 1 2"
           ,"pydat3 mdme 824 1 2"
           ,"pydat3 mdme 825 1 2"
           ,"pydat3 mdme 826 1 2"
           ,"pydat3 mdme 827 1 2"
           ,"pydat3 mdme 828 1 2"
           ,"pydat3 mdme 829 1 2"
           ,"pydat3 mdme 830 1 2"
           ,"pydat3 mdme 831 1 2"
           ,"pydat3 mdme 832 1 2"
           ,"pydat3 mdme 833 1 2"
           ,"pydat3 mdme 834 1 2"
           ,"pydat3 mdme 835 1 2"
           ,"pydat3 mdme 836 1 2"
           ,"pydat3 mdme 837 1 2"
           ,"pydat3 mdme 838 1 2"
           ,"pydat3 mdme 839 1 2"
           ,"pydat3 mdme 840 1 2"
           ,"pydat3 mdme 841 1 2"
           ,"pydat3 mdme 842 1 2"
           ,"pydat3 mdme 843 1 2"
           ,"pydat3 mdme 844 1 2"
           ,"pydat3 mdme 845 1 2"
           ,"pydat3 mdme 846 1 2"
           ,"pydat3 mdme 847 1 2"
           ,"pydat3 mdme 848 1 2"
           ,"pydat3 mdme 849 1 2"
           ,"pydat3 mdme 850 1 2"
                                         ]   


# For Ds+ -> phipi+: open Ds channels only for antiparticle Ds-
if DXChannel == "Dsbarphipi":
    PythiaB.PythiaCommand += [  "pydat3 mdme 818 1 3"
           ,"pydat3 mdme 819 1 3"
           ,"pydat3 mdme 820 1 3"
           ,"pydat3 mdme 821 1 3"
           ,"pydat3 mdme 822 1 3"
           ,"pydat3 mdme 823 1 3"
           ,"pydat3 mdme 824 1 3"
           ,"pydat3 mdme 825 1 3"
           ,"pydat3 mdme 826 1 3"
           ,"pydat3 mdme 827 1 3"
           ,"pydat3 mdme 828 1 3"
           ,"pydat3 mdme 829 1 3"
           ,"pydat3 mdme 830 1 3"
           ,"pydat3 mdme 831 1 3"
           ,"pydat3 mdme 832 1 3"
           ,"pydat3 mdme 833 1 3"
           ,"pydat3 mdme 834 1 3"
           ,"pydat3 mdme 835 1 3"
           ,"pydat3 mdme 836 1 3"
           ,"pydat3 mdme 837 1 3"
           ,"pydat3 mdme 838 1 3"
           ,"pydat3 mdme 839 1 3"
           ,"pydat3 mdme 840 1 3"
           ,"pydat3 mdme 841 1 3"
           ,"pydat3 mdme 842 1 3"
           ,"pydat3 mdme 843 1 3"
           ,"pydat3 mdme 844 1 3"
           ,"pydat3 mdme 845 1 3"
           ,"pydat3 mdme 846 1 3"
           ,"pydat3 mdme 847 1 3"
           ,"pydat3 mdme 848 1 3"
           ,"pydat3 mdme 849 1 3"
           ,"pydat3 mdme 850 1 3"
                                         ]


# For D- -> phipi-: open D channels only for particle D+
if DXChannel == "Dphipi":
    PythiaB.PythiaCommand += [  "pydat3 mdme 673 1 2"
           ,"pydat3 mdme 674 1 2"
           ,"pydat3 mdme 675 1 2"
           ,"pydat3 mdme 676 1 2"
           ,"pydat3 mdme 677 1 2"
           ,"pydat3 mdme 678 1 2"
           ,"pydat3 mdme 679 1 2"
           ,"pydat3 mdme 680 1 2"
           ,"pydat3 mdme 681 1 2"
           ,"pydat3 mdme 682 1 2"
           ,"pydat3 mdme 683 1 2"
           ,"pydat3 mdme 684 1 2"
           ,"pydat3 mdme 685 1 2"
           ,"pydat3 mdme 686 1 2"
           ,"pydat3 mdme 687 1 2"
           ,"pydat3 mdme 688 1 2"
           ,"pydat3 mdme 689 1 2"
           ,"pydat3 mdme 690 1 2"
           ,"pydat3 mdme 691 1 2"
           ,"pydat3 mdme 692 1 2"
           ,"pydat3 mdme 693 1 2"
           ,"pydat3 mdme 694 1 2"
           ,"pydat3 mdme 695 1 2"
           ,"pydat3 mdme 696 1 2"
           ,"pydat3 mdme 697 1 2"
           ,"pydat3 mdme 698 1 2"
           ,"pydat3 mdme 699 1 2"
           ,"pydat3 mdme 700 1 2"                                
           ,"pydat3 mdme 701 1 2"
           ,"pydat3 mdme 702 1 2"
           ,"pydat3 mdme 703 1 2"
           ,"pydat3 mdme 704 1 2"
           ,"pydat3 mdme 705 1 2"
           ,"pydat3 mdme 706 1 2"
           ,"pydat3 mdme 707 1 2"
           ,"pydat3 mdme 708 1 2"
           ,"pydat3 mdme 709 1 2"
           ,"pydat3 mdme 710 1 2"
           ,"pydat3 mdme 711 1 2"
           ,"pydat3 mdme 712 1 2"
           ,"pydat3 mdme 713 1 2"
           ,"pydat3 mdme 714 1 2"
           ,"pydat3 mdme 715 1 2"
           ,"pydat3 mdme 716 1 2"
           ,"pydat3 mdme 717 1 2"
           ,"pydat3 mdme 718 1 2"
           ,"pydat3 mdme 719 1 2"
           ,"pydat3 mdme 720 1 2"
           ,"pydat3 mdme 721 1 2"
           ,"pydat3 mdme 722 1 2"
           ,"pydat3 mdme 723 1 2"
           ,"pydat3 mdme 724 1 2"
           ,"pydat3 mdme 725 1 2"
           ,"pydat3 mdme 726 1 2"
           ,"pydat3 mdme 727 1 2"
           ,"pydat3 mdme 728 1 2"
           ,"pydat3 mdme 729 1 2"
           ,"pydat3 mdme 730 1 2"
           ,"pydat3 mdme 731 1 2"
           ,"pydat3 mdme 732 1 2"
           ,"pydat3 mdme 733 1 2"
           ,"pydat3 mdme 734 1 2"
           ,"pydat3 mdme 735 1 2"
                                         ]   

