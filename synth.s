	.file	"synth.c"
	.version	"01.01"
gcc2_compiled.:
.section	.rodata
	.align 32
	.type	 D,@object
	.size	 D,1088
D:
	.long 0
	.long -29
	.long 213
	.long -459
	.long 2037
	.long -5153
	.long 6574
	.long -37489
	.long 75038
	.long 37489
	.long 6574
	.long 5153
	.long 2037
	.long 459
	.long 213
	.long 29
	.long -1
	.long -31
	.long 218
	.long -519
	.long 2000
	.long -5517
	.long 5959
	.long -39336
	.long 74992
	.long 35640
	.long 7134
	.long 4788
	.long 2063
	.long 401
	.long 208
	.long 26
	.long -1
	.long -35
	.long 222
	.long -581
	.long 1952
	.long -5879
	.long 5288
	.long -41176
	.long 74856
	.long 33791
	.long 7640
	.long 4425
	.long 2080
	.long 347
	.long 202
	.long 24
	.long -1
	.long -38
	.long 225
	.long -645
	.long 1893
	.long -6237
	.long 4561
	.long -43006
	.long 74630
	.long 31947
	.long 8092
	.long 4063
	.long 2087
	.long 294
	.long 196
	.long 21
	.long -1
	.long -41
	.long 227
	.long -711
	.long 1822
	.long -6589
	.long 3776
	.long -44821
	.long 74313
	.long 30112
	.long 8492
	.long 3705
	.long 2085
	.long 244
	.long 190
	.long 19
	.long -1
	.long -45
	.long 228
	.long -779
	.long 1739
	.long -6935
	.long 2935
	.long -46617
	.long 73908
	.long 28289
	.long 8840
	.long 3351
	.long 2075
	.long 197
	.long 183
	.long 17
	.long -1
	.long -49
	.long 228
	.long -848
	.long 1644
	.long -7271
	.long 2037
	.long -48390
	.long 73415
	.long 26482
	.long 9139
	.long 3004
	.long 2057
	.long 153
	.long 176
	.long 16
	.long -2
	.long -53
	.long 227
	.long -919
	.long 1535
	.long -7597
	.long 1082
	.long -50137
	.long 72835
	.long 24694
	.long 9389
	.long 2663
	.long 2032
	.long 111
	.long 169
	.long 14
	.long -2
	.long -58
	.long 224
	.long -991
	.long 1414
	.long -7910
	.long 70
	.long -51853
	.long 72169
	.long 22929
	.long 9592
	.long 2330
	.long 2001
	.long 72
	.long 161
	.long 13
	.long -2
	.long -63
	.long 221
	.long -1064
	.long 1280
	.long -8209
	.long -998
	.long -53534
	.long 71420
	.long 21189
	.long 9750
	.long 2006
	.long 1962
	.long 36
	.long 154
	.long 11
	.long -2
	.long -68
	.long 215
	.long -1137
	.long 1131
	.long -8491
	.long -2122
	.long -55178
	.long 70590
	.long 19478
	.long 9863
	.long 1692
	.long 1919
	.long 2
	.long 147
	.long 10
	.long -3
	.long -73
	.long 208
	.long -1210
	.long 970
	.long -8755
	.long -3300
	.long -56778
	.long 69679
	.long 17799
	.long 9935
	.long 1388
	.long 1870
	.long -29
	.long 139
	.long 9
	.long -3
	.long -79
	.long 200
	.long -1283
	.long 794
	.long -8998
	.long -4533
	.long -58333
	.long 68692
	.long 16155
	.long 9966
	.long 1095
	.long 1817
	.long -57
	.long 132
	.long 8
	.long -4
	.long -85
	.long 189
	.long -1356
	.long 605
	.long -9219
	.long -5818
	.long -59838
	.long 67629
	.long 14548
	.long 9959
	.long 814
	.long 1759
	.long -83
	.long 125
	.long 7
	.long -4
	.long -91
	.long 177
	.long -1428
	.long 402
	.long -9416
	.long -7154
	.long -61289
	.long 66494
	.long 12980
	.long 9916
	.long 545
	.long 1698
	.long -106
	.long 117
	.long 7
	.long -5
	.long -97
	.long 163
	.long -1498
	.long 185
	.long -9585
	.long -8540
	.long -62684
	.long 65290
	.long 11455
	.long 9838
	.long 288
	.long 1634
	.long -127
	.long 111
	.long 6
	.long -5
	.long -104
	.long 146
	.long -1567
	.long -45
	.long -9727
	.long -9975
	.long -64019
	.long 64019
	.long 9975
	.long 9727
	.long 45
	.long 1567
	.long -146
	.long 104
	.long 5
.text
	.align 16
	.type	 dct32,@function
dct32:
	pushl %ebp
	movl %esp,%ebp
	subl $688,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 8(%ebp),%ebx
	leal -700(%ebp),%esp
	movl (%ebx),%eax
	movl 124(%ebx),%edx
	addl %eax,%edx
	movl %edx,-4(%ebp)
	movl 124(%ebx),%edx
	subl %edx,%eax
	movl %eax,-684(%ebp)
	movl $268112114,%eax
#APP
	imull -684(%ebp)
#NO_APP
	movl %eax,%ecx
	movl %edx,%edi
	sall $4,%edi
	shrl $28,%ecx
	orl %ecx,%edi
	movl %edi,-404(%ebp)
	movl 60(%ebx),%eax
	movl %eax,-684(%ebp)
	movl 64(%ebx),%ecx
	addl %ecx,%eax
	movl %eax,-8(%ebp)
	subl %ecx,-684(%ebp)
	movl $13171504,%eax
#APP
	imull -684(%ebp)
#NO_APP
	movl %eax,%ecx
	sall $4,%edx
	shrl $28,%ecx
	orl %ecx,%edx
	movl %edi,%eax
	addl %edx,%eax
	movl %eax,-96(%ebp)
	subl %edx,-404(%ebp)
	movl $267142866,%eax
#APP
	imull -404(%ebp)
#NO_APP
	movl %eax,%ecx
	sall $4,%edx
	shrl $28,%ecx
	orl %ecx,%edx
	movl %edx,-408(%ebp)
	movl -4(%ebp),%eax
	addl -8(%ebp),%eax
	movl %eax,-64(%ebp)
	movl -8(%ebp),%edx
	subl %edx,-4(%ebp)
	movl $267142866,%eax
#APP
	imull -4(%ebp)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $4,%esi
	shrl $28,%ecx
	orl %ecx,%esi
	movl %esi,-412(%ebp)
	movl 28(%ebx),%eax
	movl 96(%ebx),%edx
	addl %eax,%edx
	movl %edx,-12(%ebp)
	movl 96(%ebx),%edx
	subl %edx,%eax
	movl %eax,-684(%ebp)
	movl $198897553,%eax
#APP
	imull -684(%ebp)
#NO_APP
	movl %eax,%ecx
	movl %edx,%edi
	sall $4,%edi
	shrl $28,%ecx
	orl %ecx,%edi
	movl %edi,-416(%ebp)
	movl 32(%ebx),%eax
	movl %eax,-684(%ebp)
	movl 92(%ebx),%ecx
	addl %ecx,%eax
	movl %eax,-16(%ebp)
	subl %ecx,-684(%ebp)
	movl $180270234,%eax
#APP
	imull -684(%ebp)
#NO_APP
	movl %eax,%ecx
	sall $4,%edx
	shrl $28,%ecx
	orl %ecx,%edx
	movl %edi,%eax
	addl %edx,%eax
	movl %eax,-100(%ebp)
	subl %edx,-416(%ebp)
	movl $26311276,%eax
#APP
	imull -416(%ebp)
#NO_APP
	movl %eax,%ecx
	sall $4,%edx
	shrl $28,%ecx
	orl %ecx,%edx
	movl %edx,-420(%ebp)
	movl -12(%ebp),%eax
	addl -16(%ebp),%eax
	movl %eax,-68(%ebp)
	movl -16(%ebp),%edx
	subl %edx,-12(%ebp)
	movl $26311276,%eax
#APP
	imull -12(%ebp)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $4,%esi
	shrl $28,%ecx
	orl %ecx,%esi
	movl %esi,-424(%ebp)
	movl 12(%ebx),%eax
	movl 112(%ebx),%edx
	addl %eax,%edx
	movl %edx,-20(%ebp)
	movl 112(%ebx),%edx
	subl %edx,%eax
	movl %eax,-684(%ebp)
	movl $252743810,%eax
#APP
	imull -684(%ebp)
#NO_APP
	movl %eax,%ecx
	movl %edx,%edi
	sall $4,%edi
	shrl $28,%ecx
	orl %ecx,%edi
	movl %edi,-428(%ebp)
	movl 48(%ebx),%eax
	movl %eax,-684(%ebp)
	movl 76(%ebx),%ecx
	addl %ecx,%eax
	movl %eax,-24(%ebp)
	subl %ecx,-684(%ebp)
	movl $90433181,%eax
#APP
	imull -684(%ebp)
#NO_APP
	movl %eax,%ecx
	sall $4,%edx
	shrl $28,%ecx
	orl %ecx,%edx
	movl %edi,%eax
	addl %edx,%eax
	movl %eax,-104(%ebp)
	subl %edx,-428(%ebp)
	movl $207503414,%eax
#APP
	imull -428(%ebp)
#NO_APP
	movl %eax,%ecx
	sall $4,%edx
	shrl $28,%ecx
	orl %ecx,%edx
	movl %edx,-432(%ebp)
	movl -20(%ebp),%eax
	addl -24(%ebp),%eax
	movl %eax,-72(%ebp)
	movl -24(%ebp),%edx
	subl %edx,-20(%ebp)
	movl $207503414,%eax
#APP
	imull -20(%ebp)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $4,%esi
	shrl $28,%ecx
	orl %ecx,%esi
	movl %esi,-436(%ebp)
	movl 16(%ebx),%eax
	movl 108(%ebx),%edx
	addl %eax,%edx
	movl %edx,-28(%ebp)
	movl 108(%ebx),%edx
	subl %edx,%eax
	movl %eax,-684(%ebp)
	movl $242662778,%eax
#APP
	imull -684(%ebp)
#NO_APP
	movl %eax,%ecx
	movl %edx,%edi
	sall $4,%edi
	shrl $28,%ecx
	orl %ecx,%edi
	movl %edi,-440(%ebp)
	movl 44(%ebx),%eax
	movl %eax,-684(%ebp)
	movl 80(%ebx),%ecx
	addl %ecx,%eax
	movl %eax,-32(%ebp)
	subl %ecx,-684(%ebp)
	movl $114770946,%eax
#APP
	imull -684(%ebp)
#NO_APP
	movl %eax,%ecx
	sall $4,%edx
	shrl $28,%ecx
	orl %ecx,%edx
	movl %edi,%eax
	addl %edx,%eax
	movl %eax,-108(%ebp)
	subl %edx,-440(%ebp)
	movl $170293651,%eax
#APP
	imull -440(%ebp)
#NO_APP
	movl %eax,%ecx
	sall $4,%edx
	shrl $28,%ecx
	orl %ecx,%edx
	movl %edx,-444(%ebp)
	movl -28(%ebp),%eax
	addl -32(%ebp),%eax
	movl %eax,-76(%ebp)
	movl -32(%ebp),%edx
	subl %edx,-28(%ebp)
	movl $170293651,%eax
#APP
	imull -28(%ebp)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $4,%esi
	shrl $28,%ecx
	orl %ecx,%esi
	movl %esi,-448(%ebp)
	movl 4(%ebx),%eax
	movl 120(%ebx),%edx
	addl %eax,%edx
	movl %edx,-36(%ebp)
	movl 120(%ebx),%edx
	subl %edx,%eax
	movl %eax,-684(%ebp)
	movl $265530048,%eax
#APP
	imull -684(%ebp)
#NO_APP
	movl %eax,%ecx
	movl %edx,%edi
	sall $4,%edi
	shrl $28,%ecx
	orl %ecx,%edi
	movl %edi,-452(%ebp)
	movl 56(%ebx),%eax
	movl %eax,-684(%ebp)
	movl 68(%ebx),%ecx
	addl %ecx,%eax
	movl %eax,-40(%ebp)
	subl %ecx,-684(%ebp)
	movl $39387662,%eax
#APP
	imull -684(%ebp)
#NO_APP
	movl %eax,%ecx
	sall $4,%edx
	shrl $28,%ecx
	orl %ecx,%edx
	movl %edi,%eax
	addl %edx,%eax
	movl %eax,-112(%ebp)
	subl %edx,-452(%ebp)
	movl $256876715,%eax
#APP
	imull -452(%ebp)
#NO_APP
	movl %eax,%ecx
	sall $4,%edx
	shrl $28,%ecx
	orl %ecx,%edx
	movl %edx,-456(%ebp)
	movl -36(%ebp),%eax
	addl -40(%ebp),%eax
	movl %eax,-80(%ebp)
	movl -40(%ebp),%edx
	subl %edx,-36(%ebp)
	movl $256876715,%eax
#APP
	imull -36(%ebp)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $4,%esi
	shrl $28,%ecx
	orl %ecx,%esi
	movl %esi,-460(%ebp)
	movl 24(%ebx),%eax
	movl 100(%ebx),%edx
	addl %eax,%edx
	movl %edx,-44(%ebp)
	movl 100(%ebx),%edx
	subl %edx,%eax
	movl %eax,-684(%ebp)
	movl $215609380,%eax
#APP
	imull -684(%ebp)
#NO_APP
	movl %eax,%ecx
	movl %edx,%edi
	sall $4,%edi
	shrl $28,%ecx
	orl %ecx,%edi
	movl %edi,-464(%ebp)
	movl 36(%ebx),%eax
	movl %eax,-684(%ebp)
	movl 88(%ebx),%ecx
	addl %ecx,%eax
	movl %eax,-48(%ebp)
	subl %ecx,-684(%ebp)
	movl $159906814,%eax
#APP
	imull -684(%ebp)
#NO_APP
	movl %eax,%ecx
	sall $4,%edx
	shrl $28,%ecx
	orl %ecx,%edx
	movl %edi,%eax
	addl %edx,%eax
	movl %eax,-116(%ebp)
	subl %edx,-464(%ebp)
	movl $77922700,%eax
#APP
	imull -464(%ebp)
#NO_APP
	movl %eax,%ecx
	sall $4,%edx
	shrl $28,%ecx
	orl %ecx,%edx
	movl %edx,-468(%ebp)
	movl -44(%ebp),%eax
	addl -48(%ebp),%eax
	movl %eax,-84(%ebp)
	movl -48(%ebp),%edx
	subl %edx,-44(%ebp)
	movl $77922700,%eax
#APP
	imull -44(%ebp)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $4,%esi
	shrl $28,%ecx
	orl %ecx,%esi
	movl %esi,-472(%ebp)
	movl 8(%ebx),%eax
	movl 116(%ebx),%edx
	addl %eax,%edx
	movl %edx,-52(%ebp)
	movl 116(%ebx),%edx
	subl %edx,%eax
	movl %eax,-684(%ebp)
	movl $260390782,%eax
#APP
	imull -684(%ebp)
#NO_APP
	movl %eax,%ecx
	movl %edx,%edi
	sall $4,%edi
	shrl $28,%ecx
	orl %ecx,%edi
	movl %edi,-476(%ebp)
	movl 52(%ebx),%eax
	movl %eax,-684(%ebp)
	movl 72(%ebx),%ecx
	addl %ecx,%eax
	movl %eax,-56(%ebp)
	subl %ecx,-684(%ebp)
	movl $65224495,%eax
#APP
	imull -684(%ebp)
#NO_APP
	movl %eax,%ecx
	sall $4,%edx
	shrl $28,%ecx
	orl %ecx,%edx
	movl %edi,%eax
	addl %edx,%eax
	movl %eax,-120(%ebp)
	subl %edx,-476(%ebp)
	movl $236738937,%eax
#APP
	imull -476(%ebp)
#NO_APP
	movl %eax,%ecx
	sall $4,%edx
	shrl $28,%ecx
	orl %ecx,%edx
	movl %edx,-480(%ebp)
	movl -52(%ebp),%eax
	addl -56(%ebp),%eax
	movl %eax,-88(%ebp)
	movl -56(%ebp),%edx
	subl %edx,-52(%ebp)
	movl $236738937,%eax
#APP
	imull -52(%ebp)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $4,%esi
	shrl $28,%ecx
	orl %ecx,%esi
	movl %esi,-484(%ebp)
	movl 20(%ebx),%eax
	movl 104(%ebx),%edx
	addl %eax,%edx
	movl %edx,-60(%ebp)
	movl 104(%ebx),%edx
	subl %edx,%eax
	movl %eax,-684(%ebp)
	movl $230244771,%eax
#APP
	imull -684(%ebp)
#NO_APP
	movl %eax,%ecx
	sall $4,%edx
	shrl $28,%ecx
	orl %ecx,%edx
	movl %edx,-488(%ebp)
	movl 40(%ebx),%eax
	movl 84(%ebx),%ecx
	movl %eax,%ebx
	addl %ecx,%ebx
	subl %ecx,%eax
	movl %eax,-684(%ebp)
	movl $138003405,%eax
#APP
	imull -684(%ebp)
#NO_APP
	movl %eax,%ecx
	sall $4,%edx
	shrl $28,%ecx
	orl %ecx,%edx
	movl -488(%ebp),%ecx
	addl %edx,%ecx
	subl %edx,-488(%ebp)
	movl $126539598,%eax
#APP
	imull -488(%ebp)
#NO_APP
	sall $4,%edx
	shrl $28,%eax
	orl %eax,%edx
	movl %edx,-492(%ebp)
	movl -60(%ebp),%eax
	addl %ebx,%eax
	movl %eax,-92(%ebp)
	subl %ebx,-60(%ebp)
	movl $126539598,%eax
#APP
	imull -60(%ebp)
#NO_APP
	movl %eax,%ebx
	movl %edx,%esi
	sall $4,%esi
	shrl $28,%ebx
	orl %ebx,%esi
	movl %esi,-684(%ebp)
	movl -64(%ebp),%eax
	addl -68(%ebp),%eax
	movl %eax,-136(%ebp)
	movl -68(%ebp),%edx
	subl %edx,-64(%ebp)
	movl $263277544,%eax
#APP
	imull -64(%ebp)
#NO_APP
	movl %eax,%ebx
	movl %edx,%esi
	sall $4,%esi
	shrl $28,%ebx
	orl %ebx,%esi
	movl %esi,-496(%ebp)
	movl -72(%ebp),%eax
	addl -76(%ebp),%eax
	movl %eax,-140(%ebp)
	movl -76(%ebp),%edx
	subl %edx,-72(%ebp)
	movl $52369160,%eax
#APP
	imull -72(%ebp)
#NO_APP
	movl %eax,%ebx
	movl %edx,%esi
	sall $4,%esi
	shrl $28,%ebx
	orl %ebx,%esi
	movl %esi,-500(%ebp)
	movl -80(%ebp),%eax
	addl -84(%ebp),%eax
	movl %eax,-144(%ebp)
	movl -84(%ebp),%edx
	subl %edx,-80(%ebp)
	movl $223195925,%eax
#APP
	imull -80(%ebp)
#NO_APP
	movl %eax,%ebx
	movl %edx,%esi
	sall $4,%esi
	shrl $28,%ebx
	orl %ebx,%esi
	movl %esi,-504(%ebp)
	movl -88(%ebp),%eax
	addl -92(%ebp),%eax
	movl %eax,-148(%ebp)
	movl -92(%ebp),%edx
	subl %edx,-88(%ebp)
	movl $149134749,%eax
#APP
	imull -88(%ebp)
#NO_APP
	movl %eax,%esi
	movl %edx,%ebx
	sall $4,%ebx
	shrl $28,%esi
	orl %esi,%ebx
	movl %ebx,-508(%ebp)
	movl -96(%ebp),%eax
	addl -100(%ebp),%eax
	movl %eax,-152(%ebp)
	movl -100(%ebp),%edx
	subl %edx,-96(%ebp)
	movl $263277544,%eax
#APP
	imull -96(%ebp)
#NO_APP
	movl %eax,%esi
	movl %edx,%ebx
	sall $4,%ebx
	shrl $28,%esi
	orl %esi,%ebx
	movl %ebx,-512(%ebp)
	movl -104(%ebp),%eax
	addl -108(%ebp),%eax
	movl %eax,-156(%ebp)
	movl -108(%ebp),%edx
	subl %edx,-104(%ebp)
	movl $52369160,%eax
#APP
	imull -104(%ebp)
#NO_APP
	movl %eax,%esi
	movl %edx,%ebx
	sall $4,%ebx
	shrl $28,%esi
	orl %esi,%ebx
	movl %ebx,-516(%ebp)
	movl -112(%ebp),%eax
	addl -116(%ebp),%eax
	movl %eax,-160(%ebp)
	movl -116(%ebp),%edx
	subl %edx,-112(%ebp)
	movl $223195925,%eax
#APP
	imull -112(%ebp)
#NO_APP
	movl %eax,%esi
	movl %edx,%ebx
	sall $4,%ebx
	shrl $28,%esi
	orl %esi,%ebx
	movl -120(%ebp),%eax
	addl %ecx,%eax
	movl %eax,-164(%ebp)
	subl %ecx,-120(%ebp)
	movl $149134749,%eax
#APP
	imull -120(%ebp)
#NO_APP
	movl %eax,%esi
	movl %edx,%ecx
	sall $4,%ecx
	shrl $28,%esi
	orl %esi,%ecx
	movl %ecx,-520(%ebp)
	movl -412(%ebp),%eax
	addl -424(%ebp),%eax
	movl %eax,-172(%ebp)
	movl -424(%ebp),%edx
	subl %edx,-412(%ebp)
	movl $263277544,%eax
#APP
	imull -412(%ebp)
#NO_APP
	movl %eax,%esi
	movl %edx,%ecx
	sall $4,%ecx
	shrl $28,%esi
	orl %esi,%ecx
	movl %ecx,-524(%ebp)
	movl -436(%ebp),%eax
	addl -448(%ebp),%eax
	movl %eax,-176(%ebp)
	movl -448(%ebp),%edx
	subl %edx,-436(%ebp)
	movl $52369160,%eax
#APP
	imull -436(%ebp)
#NO_APP
	movl %eax,%esi
	movl %edx,%ecx
	sall $4,%ecx
	shrl $28,%esi
	orl %esi,%ecx
	movl %ecx,-528(%ebp)
	movl -460(%ebp),%eax
	addl -472(%ebp),%eax
	movl %eax,-180(%ebp)
	movl -472(%ebp),%edx
	subl %edx,-460(%ebp)
	movl $223195925,%eax
#APP
	imull -460(%ebp)
#NO_APP
	movl %eax,%esi
	movl %edx,%ecx
	sall $4,%ecx
	shrl $28,%esi
	orl %esi,%ecx
	movl -484(%ebp),%eax
	addl -684(%ebp),%eax
	movl %eax,-184(%ebp)
	movl -684(%ebp),%edx
	subl %edx,-484(%ebp)
	movl $149134749,%eax
#APP
	imull -484(%ebp)
#NO_APP
	movl %eax,%esi
	movl %edx,%edi
	sall $4,%edi
	shrl $28,%esi
	orl %esi,%edi
	movl %edi,-532(%ebp)
	movl -408(%ebp),%eax
	addl -420(%ebp),%eax
	movl %eax,-192(%ebp)
	movl -420(%ebp),%edx
	subl %edx,-408(%ebp)
	movl $263277544,%eax
#APP
	imull -408(%ebp)
#NO_APP
	movl %eax,%edi
	sall $4,%edx
	shrl $28,%edi
	orl %edi,%edx
	movl %edx,-536(%ebp)
	movl -432(%ebp),%eax
	addl -444(%ebp),%eax
	movl %eax,-196(%ebp)
	movl -444(%ebp),%edx
	subl %edx,-432(%ebp)
	movl $52369160,%eax
#APP
	imull -432(%ebp)
#NO_APP
	movl %eax,%edi
	sall $4,%edx
	shrl $28,%edi
	orl %edi,%edx
	movl %edx,-540(%ebp)
	movl -456(%ebp),%eax
	addl -468(%ebp),%eax
	movl %eax,-200(%ebp)
	movl -468(%ebp),%edx
	subl %edx,-456(%ebp)
	movl $223195925,%eax
#APP
	imull -456(%ebp)
#NO_APP
	movl %eax,%edi
	sall $4,%edx
	shrl $28,%edi
	orl %edi,%edx
	movl %edx,-544(%ebp)
	movl -480(%ebp),%eax
	addl -492(%ebp),%eax
	movl %eax,-204(%ebp)
	movl -492(%ebp),%edx
	subl %edx,-480(%ebp)
	movl $149134749,%eax
#APP
	imull -480(%ebp)
#NO_APP
	movl %eax,%edi
	sall $4,%edx
	shrl $28,%edi
	orl %edi,%edx
	movl %edx,-548(%ebp)
	movl -136(%ebp),%eax
	addl -140(%ebp),%eax
	movl %eax,-240(%ebp)
	movl -144(%ebp),%edx
	addl -148(%ebp),%edx
	movl %edx,-244(%ebp)
	movl -152(%ebp),%eax
	addl -156(%ebp),%eax
	movl %eax,-248(%ebp)
	movl -160(%ebp),%edx
	addl -164(%ebp),%edx
	movl %edx,-252(%ebp)
	addl %edx,%eax
	movl %eax,-672(%ebp)
	movl -172(%ebp),%eax
	addl -176(%ebp),%eax
	movl %eax,-256(%ebp)
	movl -180(%ebp),%edx
	addl -184(%ebp),%edx
	movl %edx,-260(%ebp)
	addl %edx,%eax
	movl %eax,-676(%ebp)
	movl -192(%ebp),%eax
	addl -196(%ebp),%eax
	movl %eax,-268(%ebp)
	movl -200(%ebp),%edx
	addl -204(%ebp),%edx
	movl %edx,-272(%ebp)
	addl %edx,%eax
	movl %eax,-128(%ebp)
	addl %eax,%eax
	movl -672(%ebp),%edx
	subl %edx,%eax
	movl %eax,-124(%ebp)
	movl -496(%ebp),%eax
	addl -500(%ebp),%eax
	movl %eax,-280(%ebp)
	movl -504(%ebp),%edx
	addl -508(%ebp),%edx
	movl %edx,-284(%ebp)
	addl %edx,%eax
	movl %eax,-680(%ebp)
	movl -512(%ebp),%eax
	addl -516(%ebp),%eax
	movl %eax,-292(%ebp)
	movl -520(%ebp),%edx
	addl %ebx,%edx
	movl %edx,-296(%ebp)
	addl %edx,%eax
	movl %eax,-212(%ebp)
	addl %eax,%eax
	movl -124(%ebp),%edx
	subl %edx,%eax
	movl %eax,-132(%ebp)
	movl -524(%ebp),%eax
	addl -528(%ebp),%eax
	movl %eax,-304(%ebp)
	movl -532(%ebp),%edx
	addl %ecx,%edx
	movl %edx,-308(%ebp)
	addl %edx,%eax
	movl %eax,-220(%ebp)
	addl %eax,%eax
	movl -676(%ebp),%edx
	subl %edx,%eax
	movl %eax,-188(%ebp)
	movl -536(%ebp),%eax
	addl -540(%ebp),%eax
	movl %eax,-316(%ebp)
	movl -544(%ebp),%edx
	addl -548(%ebp),%edx
	movl %edx,-320(%ebp)
	addl %edx,%eax
	movl %eax,-228(%ebp)
	addl %eax,%eax
	movl -128(%ebp),%edx
	subl %edx,%eax
	movl %eax,-684(%ebp)
	addl %eax,%eax
	movl -132(%ebp),%edx
	subl %edx,%eax
	movl %eax,-168(%ebp)
	movl -140(%ebp),%eax
	subl %eax,-136(%ebp)
	movl $248002024,%eax
#APP
	imull -136(%ebp)
#NO_APP
	movl %edx,%edi
	sall $4,%edi
	shrl $28,%eax
	orl %eax,%edi
	movl %edi,-552(%ebp)
	movl -148(%ebp),%eax
	subl %eax,-144(%ebp)
	movl $102725802,%eax
#APP
	imull -144(%ebp)
#NO_APP
	movl %edx,%edi
	sall $4,%edi
	shrl $28,%eax
	orl %eax,%edi
	movl %edi,-556(%ebp)
	movl -552(%ebp),%eax
	addl %edi,%eax
	movl %eax,-332(%ebp)
	movl -156(%ebp),%edx
	subl %edx,-152(%ebp)
	movl $248002024,%eax
#APP
	imull -152(%ebp)
#NO_APP
	movl %edx,%edi
	sall $4,%edi
	shrl $28,%eax
	orl %eax,%edi
	movl %edi,-560(%ebp)
	movl -164(%ebp),%eax
	subl %eax,-160(%ebp)
	movl $102725802,%eax
#APP
	imull -160(%ebp)
#NO_APP
	sall $4,%edx
	shrl $28,%eax
	orl %eax,%edx
	movl %edx,-568(%ebp)
	movl %edi,%eax
	addl %edx,%eax
	movl %eax,-336(%ebp)
	addl %eax,%eax
	movl -168(%ebp),%edx
	subl %edx,%eax
	movl %eax,-208(%ebp)
	movl -176(%ebp),%eax
	subl %eax,-172(%ebp)
	movl $248002024,%eax
#APP
	imull -172(%ebp)
#NO_APP
	movl %eax,%edi
	sall $4,%edx
	shrl $28,%edi
	orl %edi,%edx
	movl %edx,-572(%ebp)
	movl -184(%ebp),%eax
	subl %eax,-180(%ebp)
	movl $102725802,%eax
#APP
	imull -180(%ebp)
#NO_APP
	movl %eax,%esi
	sall $4,%edx
	shrl $28,%esi
	orl %esi,%edx
	movl %edx,-580(%ebp)
	movl -572(%ebp),%eax
	addl %edx,%eax
	movl %eax,-344(%ebp)
	addl %eax,%eax
	movl -188(%ebp),%edx
	subl %edx,%eax
	movl %eax,-224(%ebp)
	movl -196(%ebp),%eax
	subl %eax,-192(%ebp)
	movl $248002024,%eax
#APP
	imull -192(%ebp)
#NO_APP
	movl %edx,%esi
	sall $4,%esi
	shrl $28,%eax
	orl %eax,%esi
	movl %esi,-588(%ebp)
	movl -204(%ebp),%eax
	subl %eax,-200(%ebp)
	movl $102725802,%eax
#APP
	imull -200(%ebp)
#NO_APP
	sall $4,%edx
	shrl $28,%eax
	orl %eax,%edx
	movl %edx,-600(%ebp)
	movl %esi,%eax
	addl %edx,%eax
	movl %eax,-352(%ebp)
	addl %eax,%eax
	movl -684(%ebp),%edx
	subl %edx,%eax
	movl %eax,-232(%ebp)
	addl %eax,%eax
	movl -208(%ebp),%edx
	subl %edx,%eax
	movl %eax,-216(%ebp)
	movl -500(%ebp),%eax
	subl %eax,-496(%ebp)
	movl $248002024,%eax
#APP
	imull -496(%ebp)
#NO_APP
	sall $4,%edx
	shrl $28,%eax
	orl %eax,%edx
	movl %edx,-608(%ebp)
	movl -508(%ebp),%eax
	subl %eax,-504(%ebp)
	movl $102725802,%eax
#APP
	imull -504(%ebp)
#NO_APP
	sall $4,%edx
	shrl $28,%eax
	orl %eax,%edx
	movl %edx,-616(%ebp)
	movl -608(%ebp),%eax
	addl %edx,%eax
	movl %eax,-356(%ebp)
	addl %eax,%eax
	movl -680(%ebp),%edx
	subl %edx,%eax
	movl %eax,-288(%ebp)
	movl -284(%ebp),%eax
	subl %eax,-280(%ebp)
	movl $189812531,%eax
#APP
	imull -280(%ebp)
#NO_APP
	sall $4,%edx
	shrl $28,%eax
	orl %eax,%edx
	addl %edx,%edx
	movl -288(%ebp),%eax
	subl %eax,%edx
	movl %edx,-360(%ebp)
	movl -516(%ebp),%edx
	subl %edx,-512(%ebp)
	movl $248002024,%eax
#APP
	imull -512(%ebp)
#NO_APP
	sall $4,%edx
	shrl $28,%eax
	orl %eax,%edx
	movl %edx,-624(%ebp)
	subl -520(%ebp),%ebx
	movl $102725802,%eax
#APP
	imull %ebx
#NO_APP
	movl %edx,%ebx
	sall $4,%ebx
	shrl $28,%eax
	orl %eax,%ebx
	movl %ebx,-628(%ebp)
	movl -624(%ebp),%eax
	addl %ebx,%eax
	movl %eax,-364(%ebp)
	addl %eax,%eax
	movl -212(%ebp),%edx
	subl %edx,%eax
	movl %eax,-684(%ebp)
	addl %eax,%eax
	movl -216(%ebp),%edx
	subl %edx,%eax
	movl %eax,-236(%ebp)
	movl -296(%ebp),%eax
	subl %eax,-292(%ebp)
	movl $189812531,%eax
#APP
	imull -292(%ebp)
#NO_APP
	movl %edx,%ebx
	sall $4,%ebx
	shrl $28,%eax
	orl %eax,%ebx
	addl %ebx,%ebx
	movl -684(%ebp),%eax
	subl %eax,%ebx
	movl %ebx,-368(%ebp)
	movl -528(%ebp),%edx
	subl %edx,-524(%ebp)
	movl $248002024,%eax
#APP
	imull -524(%ebp)
#NO_APP
	movl %edx,%ebx
	sall $4,%ebx
	shrl $28,%eax
	orl %eax,%ebx
	movl %ebx,-636(%ebp)
	subl -532(%ebp),%ecx
	movl $102725802,%eax
#APP
	imull %ecx
#NO_APP
	movl %eax,%ebx
	movl %edx,%ecx
	sall $4,%ecx
	shrl $28,%ebx
	orl %ebx,%ecx
	movl %ecx,-640(%ebp)
	movl -636(%ebp),%eax
	addl %ecx,%eax
	movl %eax,-376(%ebp)
	addl %eax,%eax
	movl -220(%ebp),%edx
	subl %edx,%eax
	movl %eax,-684(%ebp)
	addl %eax,%eax
	movl -224(%ebp),%edx
	subl %edx,%eax
	movl %eax,-264(%ebp)
	movl -260(%ebp),%eax
	subl %eax,-256(%ebp)
	movl $189812531,%eax
#APP
	imull -256(%ebp)
#NO_APP
	movl %eax,%ebx
	movl %edx,%ecx
	sall $4,%ecx
	shrl $28,%ebx
	orl %ebx,%ecx
	addl %ecx,%ecx
	movl -264(%ebp),%eax
	subl %eax,%ecx
	movl %ecx,-312(%ebp)
	movl -308(%ebp),%edx
	subl %edx,-304(%ebp)
	movl $189812531,%eax
#APP
	imull -304(%ebp)
#NO_APP
	movl %eax,%ebx
	movl %edx,%ecx
	sall $4,%ecx
	shrl $28,%ebx
	orl %ebx,%ecx
	addl %ecx,%ecx
	movl -684(%ebp),%eax
	subl %eax,%ecx
	movl %ecx,-380(%ebp)
	movl %ecx,%edx
	addl %edx,%edx
	movl -312(%ebp),%eax
	subl %eax,%edx
	movl %edx,-348(%ebp)
	movl -580(%ebp),%edx
	subl %edx,-572(%ebp)
	movl $189812531,%eax
#APP
	imull -572(%ebp)
#NO_APP
	movl %eax,%ecx
	movl %edx,%ebx
	sall $4,%ebx
	shrl $28,%ecx
	orl %ecx,%ebx
	movl %ebx,%ecx
	addl %ebx,%ecx
	subl -344(%ebp),%ecx
	addl %ecx,%ecx
	movl -348(%ebp),%eax
	subl %eax,%ecx
	movl %ecx,-384(%ebp)
	movl -540(%ebp),%edx
	subl %edx,-536(%ebp)
	movl $248002024,%eax
#APP
	imull -536(%ebp)
#NO_APP
	movl %eax,%ecx
	movl %edx,%ebx
	sall $4,%ebx
	shrl $28,%ecx
	orl %ecx,%ebx
	movl %ebx,-644(%ebp)
	movl -548(%ebp),%eax
	subl %eax,-544(%ebp)
	movl $102725802,%eax
#APP
	imull -544(%ebp)
#NO_APP
	movl %eax,%ecx
	movl %edx,%ebx
	sall $4,%ebx
	shrl $28,%ecx
	orl %ecx,%ebx
	movl %ebx,-648(%ebp)
	movl -644(%ebp),%eax
	addl %ebx,%eax
	movl %eax,-388(%ebp)
	addl %eax,%eax
	movl -228(%ebp),%edx
	subl %edx,%eax
	movl %eax,-684(%ebp)
	movl %eax,%ebx
	addl %eax,%ebx
	subl -232(%ebp),%ebx
	movl -272(%ebp),%eax
	subl %eax,-268(%ebp)
	movl $189812531,%eax
#APP
	imull -268(%ebp)
#NO_APP
	movl %eax,%ecx
	sall $4,%edx
	shrl $28,%ecx
	orl %ecx,%edx
	addl %edx,%edx
	subl %ebx,%edx
	movl %edx,-324(%ebp)
	addl %ebx,%ebx
	subl -236(%ebp),%ebx
	movl -252(%ebp),%eax
	subl %eax,-248(%ebp)
	movl $189812531,%eax
#APP
	imull -248(%ebp)
#NO_APP
	movl %edx,%ecx
	sall $4,%ecx
	shrl $28,%eax
	orl %eax,%ecx
	addl %ecx,%ecx
	subl %ebx,%ecx
	movl %ecx,-276(%ebp)
	movl -324(%ebp),%eax
	addl %eax,%eax
	subl %ecx,%eax
	movl %eax,-300(%ebp)
	movl -368(%ebp),%edx
	addl %edx,%edx
	subl %eax,%edx
	movl %edx,-328(%ebp)
	movl -320(%ebp),%eax
	subl %eax,-316(%ebp)
	movl $189812531,%eax
#APP
	imull -316(%ebp)
#NO_APP
	movl %edx,%ecx
	sall $4,%ecx
	shrl $28,%eax
	orl %eax,%ecx
	addl %ecx,%ecx
	movl -684(%ebp),%eax
	subl %eax,%ecx
	movl %ecx,-392(%ebp)
	movl %ecx,%edx
	addl %edx,%edx
	movl -324(%ebp),%eax
	subl %eax,%edx
	movl %edx,-684(%ebp)
	addl %edx,%edx
	movl -328(%ebp),%eax
	subl %eax,%edx
	movl %edx,-340(%ebp)
	movl -568(%ebp),%edx
	subl %edx,-560(%ebp)
	movl $189812531,%eax
#APP
	imull -560(%ebp)
#NO_APP
	movl %eax,%ecx
	movl %edx,%edi
	sall $4,%edi
	shrl $28,%ecx
	orl %ecx,%edi
	movl %edi,%ecx
	addl %edi,%ecx
	subl -336(%ebp),%ecx
	addl %ecx,%ecx
	subl -340(%ebp),%ecx
	movl -600(%ebp),%eax
	subl %eax,-588(%ebp)
	movl $189812531,%eax
#APP
	imull -588(%ebp)
#NO_APP
	movl %eax,%esi
	movl %edx,%edi
	sall $4,%edi
	shrl $28,%esi
	orl %esi,%edi
	addl %edi,%edi
	movl -352(%ebp),%eax
	subl %eax,%edi
	movl %edi,-660(%ebp)
	movl %edi,%edx
	addl %edx,%edx
	movl -684(%ebp),%eax
	subl %eax,%edx
	movl %edx,-396(%ebp)
	addl %edx,%edx
	subl %ecx,%edx
	movl %edx,-372(%ebp)
	movl -628(%ebp),%edx
	subl %edx,-624(%ebp)
	movl $189812531,%eax
#APP
	imull -624(%ebp)
#NO_APP
	sall $4,%edx
	shrl $28,%eax
	orl %eax,%edx
	addl %edx,%edx
	movl -364(%ebp),%eax
	subl %eax,%edx
	addl %edx,%edx
	movl -368(%ebp),%eax
	subl %eax,%edx
	addl %edx,%edx
	movl -372(%ebp),%eax
	subl %eax,%edx
	movl %edx,-400(%ebp)
	movl -240(%ebp),%eax
	movl 16(%ebp),%edx
	addl -244(%ebp),%eax
	movl %eax,60(%edx)
	movl -672(%ebp),%eax
	movl %eax,56(%edx)
	movl -676(%ebp),%eax
	movl %eax,52(%edx)
	movl -124(%ebp),%eax
	movl %eax,48(%edx)
	movl -680(%ebp),%eax
	movl %eax,44(%edx)
	movl -132(%ebp),%eax
	movl %eax,40(%edx)
	movl -188(%ebp),%eax
	movl %eax,36(%edx)
	movl -168(%ebp),%eax
	movl %eax,32(%edx)
	movl -332(%ebp),%eax
	movl %eax,28(%edx)
	movl -208(%ebp),%eax
	movl %eax,24(%edx)
	movl -224(%ebp),%eax
	movl %eax,20(%edx)
	movl -216(%ebp),%eax
	movl %eax,16(%edx)
	movl -288(%ebp),%eax
	movl %eax,12(%edx)
	movl -236(%ebp),%eax
	movl %eax,8(%edx)
	movl -264(%ebp),%eax
	movl %eax,4(%edx)
	movl %ebx,(%edx)
	movl -244(%ebp),%edx
	subl %edx,-240(%ebp)
	movl $189812531,%eax
#APP
	imull -240(%ebp)
#NO_APP
	movl %eax,%ebx
	sall $4,%edx
	shrl $28,%ebx
	movl 12(%ebp),%eax
	orl %edx,%ebx
	movl %ebx,(%eax)
	movl -276(%ebp),%edx
	movl %edx,4(%eax)
	movl -312(%ebp),%edx
	movl %edx,8(%eax)
	movl -300(%ebp),%edx
	movl %edx,12(%eax)
	movl -360(%ebp),%edx
	movl %edx,16(%eax)
	movl -328(%ebp),%edx
	movl %edx,20(%eax)
	movl -348(%ebp),%edx
	movl %edx,24(%eax)
	movl -340(%ebp),%edx
	movl %edx,28(%eax)
	movl -556(%ebp),%eax
	subl %eax,-552(%ebp)
	movl $189812531,%eax
#APP
	imull -552(%ebp)
#NO_APP
	movl %eax,%ebx
	sall $4,%edx
	shrl $28,%ebx
	orl %ebx,%edx
	addl %edx,%edx
	movl 12(%ebp),%eax
	subl -332(%ebp),%edx
	movl %edx,32(%eax)
	movl %ecx,36(%eax)
	movl -384(%ebp),%edx
	movl %edx,40(%eax)
	movl -372(%ebp),%edx
	movl %edx,44(%eax)
	movl -616(%ebp),%eax
	subl %eax,-608(%ebp)
	movl $189812531,%eax
#APP
	imull -608(%ebp)
#NO_APP
	movl %eax,%ecx
	movl %edx,%ebx
	sall $4,%ebx
	shrl $28,%ecx
	orl %ecx,%ebx
	addl %ebx,%ebx
	movl -356(%ebp),%eax
	subl %eax,%ebx
	movl %ebx,-684(%ebp)
	movl %ebx,%edx
	addl %edx,%edx
	movl 12(%ebp),%eax
	subl -360(%ebp),%edx
	movl %edx,48(%eax)
	movl -400(%ebp),%edx
	movl %edx,52(%eax)
	movl -640(%ebp),%eax
	subl %eax,-636(%ebp)
	movl $189812531,%eax
#APP
	imull -636(%ebp)
#NO_APP
	movl %eax,%ecx
	movl %edx,%ebx
	sall $4,%ebx
	shrl $28,%ecx
	orl %ecx,%ebx
	addl %ebx,%ebx
	movl -376(%ebp),%eax
	subl %eax,%ebx
	movl %ebx,-684(%ebp)
	movl %ebx,%edx
	addl %edx,%edx
	movl -380(%ebp),%eax
	subl %eax,%edx
	addl %edx,%edx
	movl 12(%ebp),%eax
	subl -384(%ebp),%edx
	movl %edx,56(%eax)
	movl -648(%ebp),%edx
	subl %edx,-644(%ebp)
	movl $189812531,%eax
#APP
	imull -644(%ebp)
#NO_APP
	movl %eax,%ecx
	movl %edx,%ebx
	sall $4,%ebx
	shrl $28,%ecx
	orl %ecx,%ebx
	addl %ebx,%ebx
	movl -388(%ebp),%eax
	subl %eax,%ebx
	movl %ebx,-684(%ebp)
	movl %ebx,%edx
	addl %edx,%edx
	movl -392(%ebp),%eax
	subl %eax,%edx
	addl %edx,%edx
	movl -396(%ebp),%eax
	subl %eax,%edx
	addl %edx,%edx
	movl 12(%ebp),%eax
	subl -400(%ebp),%edx
	movl %edx,60(%eax)
	popl %ebx
	popl %esi
	popl %edi
	movl %ebp,%esp
	popl %ebp
	ret
.Lfe1:
	.size	 dct32,.Lfe1-dct32
	.align 16
.globl mad_synth_frame
	.type	 mad_synth_frame,@function
mad_synth_frame:
	pushl %ebp
	movl %esp,%ebp
	subl $140,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 12(%ebp),%eax
	cmpl $0,4(%eax)
	je .L9
	movl $2,-4(%ebp)
	jmp .L10
	.p2align 4,,7
.L9:
	movl $1,-4(%ebp)
.L10:
	movl 12(%ebp),%edx
	movl (%edx),%ecx
	cmpl $1,%ecx
	je .L11
	cmpl $3,%ecx
	jne .L13
	testb $16,25(%edx)
	je .L13
	movl $18,-8(%ebp)
	jmp .L12
	.p2align 4,,7
.L13:
	movl $36,-8(%ebp)
	jmp .L12
	.p2align 4,,7
.L11:
	movl $12,-8(%ebp)
.L12:
	movl $0,-16(%ebp)
	movl -8(%ebp),%eax
	cmpl %eax,-16(%ebp)
	jae .L16
	.p2align 4,,7
.L18:
	movl 8(%ebp),%edx
	xorl %eax,%eax
	movw 4096(%edx),%ax
	movl %eax,-28(%ebp)
	movl -28(%ebp),%edx
	incl %edx
	movl 8(%ebp),%eax
	andl $15,%edx
	movw %dx,4096(%eax)
	movl -28(%ebp),%edx
	movl %edx,-32(%ebp)
	andl $14,-32(%ebp)
	decl %edx
	andl $15,%edx
	orl $1,%edx
	movl %edx,-36(%ebp)
	movl -32(%ebp),%eax
	addl $14,%eax
	andl $15,%eax
	movl %eax,-40(%ebp)
	movl -32(%ebp),%edx
	addl $12,%edx
	andl $15,%edx
	movl %edx,-44(%ebp)
	movl -32(%ebp),%eax
	addl $10,%eax
	andl $15,%eax
	movl %eax,-48(%ebp)
	movl -32(%ebp),%edx
	addl $8,%edx
	andl $15,%edx
	movl %edx,-52(%ebp)
	movl -32(%ebp),%eax
	addl $6,%eax
	andl $15,%eax
	movl %eax,-56(%ebp)
	movl -32(%ebp),%edx
	addl $4,%edx
	andl $15,%edx
	movl %edx,-60(%ebp)
	movl -32(%ebp),%eax
	addl $2,%eax
	andl $15,%eax
	movl %eax,-64(%ebp)
	movl -36(%ebp),%edx
	addl $14,%edx
	andl $15,%edx
	movl %edx,-68(%ebp)
	movl -36(%ebp),%eax
	addl $12,%eax
	andl $15,%eax
	movl %eax,-72(%ebp)
	movl -36(%ebp),%edx
	addl $10,%edx
	andl $15,%edx
	movl %edx,-76(%ebp)
	movl -36(%ebp),%eax
	addl $8,%eax
	andl $15,%eax
	movl %eax,-80(%ebp)
	movl -36(%ebp),%edx
	addl $6,%edx
	andl $15,%edx
	movl %edx,-84(%ebp)
	movl -36(%ebp),%eax
	addl $4,%eax
	andl $15,%eax
	movl %eax,-88(%ebp)
	movl -36(%ebp),%edx
	addl $2,%edx
	andl $15,%edx
	movl %edx,-92(%ebp)
	movl $0,-12(%ebp)
	movl -4(%ebp),%eax
	cmpl %eax,-12(%ebp)
	jae .L17
	movl 12(%ebp),%edx
	addl $40,%edx
	movl %edx,-116(%ebp)
	movl $0,-120(%ebp)
	.p2align 4,,7
.L22:
	movl -12(%ebp),%eax
	movl %eax,-136(%ebp)
	sall $11,-136(%ebp)
	movl -136(%ebp),%edx
	addl 8(%ebp),%edx
	movl %edx,-96(%ebp)
	addl $1024,%edx
	movl %edx,-100(%ebp)
	movl -28(%ebp),%eax
	movl %eax,-136(%ebp)
	sall $6,-136(%ebp)
	addl -136(%ebp),%edx
	pushl %edx
	movl -136(%ebp),%edx
	addl -96(%ebp),%edx
	pushl %edx
	movl -16(%ebp),%ebx
	sall $7,%ebx
	movl -116(%ebp),%eax
	addl %ebx,%eax
	pushl %eax
	call dct32
	movl -120(%ebp),%edx
	movl 8(%ebp),%eax
	leal 4100(%eax,%edx),%edx
	addl %edx,%ebx
	movl %ebx,-24(%ebp)
	addl $12,%esp
	testb $1,-28(%ebp)
	je .L23
	movl -96(%ebp),%ebx
	addl $64,-96(%ebp)
	jmp .L24
	.p2align 4,,7
.L23:
	movl -96(%ebp),%ebx
	addl $64,%ebx
	addl $64,-100(%ebp)
.L24:
	movl $D,%esi
	movl -96(%ebp),%eax
	movl -32(%ebp),%edx
	movl (%eax),%eax
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %edx,%ecx
	movl %ecx,%edi
	sall $16,%edi
	shrl $16,%eax
	orl %eax,%edi
	movl -96(%ebp),%eax
	movl -40(%ebp),%edx
	movl 128(%eax),%eax
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	sall $16,%edx
	shrl $16,%ecx
	orl %ecx,%edx
	addl %edx,%edi
	movl -96(%ebp),%eax
	movl -44(%ebp),%edx
	movl 256(%eax),%eax
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	sall $16,%edx
	shrl $16,%ecx
	orl %ecx,%edx
	addl %edx,%edi
	movl -96(%ebp),%eax
	movl -48(%ebp),%edx
	movl 384(%eax),%eax
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	sall $16,%edx
	shrl $16,%ecx
	orl %ecx,%edx
	addl %edx,%edi
	movl -96(%ebp),%eax
	movl -52(%ebp),%edx
	movl 512(%eax),%eax
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	sall $16,%edx
	shrl $16,%ecx
	orl %ecx,%edx
	addl %edx,%edi
	movl -96(%ebp),%eax
	movl -56(%ebp),%edx
	movl 640(%eax),%eax
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	sall $16,%edx
	shrl $16,%ecx
	orl %ecx,%edx
	addl %edx,%edi
	movl -96(%ebp),%eax
	movl -60(%ebp),%edx
	movl 768(%eax),%eax
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	sall $16,%edx
	shrl $16,%ecx
	orl %ecx,%edx
	addl %edx,%edi
	movl -96(%ebp),%eax
	movl -64(%ebp),%edx
	movl 896(%eax),%eax
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	sall $16,%edx
	shrl $16,%ecx
	orl %ecx,%edx
	addl %edx,%edi
	movl (%ebx),%eax
	movl -36(%ebp),%edx
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	sall $16,%edx
	shrl $16,%ecx
	orl %ecx,%edx
	subl %edx,%edi
	movl 128(%ebx),%eax
	movl -68(%ebp),%edx
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	sall $16,%edx
	shrl $16,%ecx
	orl %ecx,%edx
	subl %edx,%edi
	movl 256(%ebx),%eax
	movl -72(%ebp),%edx
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	sall $16,%edx
	shrl $16,%ecx
	orl %ecx,%edx
	subl %edx,%edi
	movl 384(%ebx),%eax
	movl -76(%ebp),%edx
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	sall $16,%edx
	shrl $16,%ecx
	orl %ecx,%edx
	subl %edx,%edi
	movl 512(%ebx),%eax
	movl -80(%ebp),%edx
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	sall $16,%edx
	shrl $16,%ecx
	orl %ecx,%edx
	subl %edx,%edi
	movl 640(%ebx),%eax
	movl -84(%ebp),%edx
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	sall $16,%edx
	shrl $16,%ecx
	orl %ecx,%edx
	subl %edx,%edi
	movl 768(%ebx),%eax
	movl -88(%ebp),%edx
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	sall $16,%edx
	shrl $16,%ecx
	orl %ecx,%edx
	subl %edx,%edi
	movl 896(%ebx),%eax
	movl -92(%ebp),%edx
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%ebx
	sall $16,%ebx
	shrl $16,%ecx
	orl %ecx,%ebx
	subl %ebx,%edi
	movl -24(%ebp),%eax
	movl $D+64,%ebx
	movl %edi,(%eax)
	addl $4,-96(%ebp)
	movl $1,-20(%ebp)
	movl $15,-104(%ebp)
	movl -92(%ebp),%edx
	subl %edx,-104(%ebp)
	movl -100(%ebp),%eax
	addl $896,%eax
	movl %eax,-108(%ebp)
	movl -96(%ebp),%edx
	addl $896,%edx
	movl %edx,-112(%ebp)
	movl -100(%ebp),%eax
	addl $768,%eax
	movl %eax,-124(%ebp)
	movl -96(%ebp),%edx
	addl $768,%edx
	movl %edx,-128(%ebp)
	.p2align 4,,7
.L28:
	movl -96(%ebp),%eax
	movl -32(%ebp),%edx
	movl (%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	movl %esi,%edi
	sall $16,%edi
	shrl $16,%ecx
	orl %ecx,%edi
	movl $15,-136(%ebp)
	movl -32(%ebp),%edx
	subl %edx,-136(%ebp)
	movl -96(%ebp),%eax
	movl -136(%ebp),%edx
	movl (%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%esi
	movl %edx,%ecx
	sall $16,%ecx
	shrl $16,%esi
	orl %esi,%ecx
	movl -128(%ebp),%eax
	movl -40(%ebp),%edx
	movl $15,-136(%ebp)
	movl -640(%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%esi
	sall $16,%edx
	shrl $16,%esi
	orl %esi,%edx
	addl %edx,%edi
	movl -40(%ebp),%edx
	subl %edx,-136(%ebp)
	movl -128(%ebp),%eax
	movl -136(%ebp),%edx
	movl -640(%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%esi
	sall $16,%edx
	shrl $16,%esi
	orl %esi,%edx
	addl %edx,%ecx
	movl %ecx,-140(%ebp)
	movl -128(%ebp),%eax
	movl -44(%ebp),%edx
	movl -512(%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	addl %esi,%edi
	movl $15,-136(%ebp)
	movl -44(%ebp),%edx
	subl %edx,-136(%ebp)
	movl -128(%ebp),%eax
	movl -136(%ebp),%edx
	movl -512(%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	addl %esi,-140(%ebp)
	movl -128(%ebp),%eax
	movl -48(%ebp),%edx
	movl $15,-136(%ebp)
	movl -384(%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	addl %esi,%edi
	movl -48(%ebp),%edx
	subl %edx,-136(%ebp)
	movl -128(%ebp),%eax
	movl -136(%ebp),%edx
	movl -384(%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	addl %esi,-140(%ebp)
	movl -128(%ebp),%eax
	movl -52(%ebp),%edx
	movl -256(%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	addl %esi,%edi
	movl $15,-136(%ebp)
	movl -52(%ebp),%edx
	subl %edx,-136(%ebp)
	movl -128(%ebp),%eax
	movl -136(%ebp),%edx
	movl -256(%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	addl %esi,-140(%ebp)
	movl -128(%ebp),%eax
	movl -56(%ebp),%edx
	movl $15,-136(%ebp)
	movl -128(%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	addl %esi,%edi
	movl -56(%ebp),%edx
	subl %edx,-136(%ebp)
	movl -128(%ebp),%eax
	movl -136(%ebp),%edx
	movl -128(%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	addl %esi,-140(%ebp)
	movl -128(%ebp),%eax
	movl -60(%ebp),%edx
	movl (%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	addl %esi,%edi
	movl $15,-136(%ebp)
	movl -60(%ebp),%edx
	subl %edx,-136(%ebp)
	movl -128(%ebp),%eax
	movl -136(%ebp),%edx
	movl (%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	addl %esi,-140(%ebp)
	movl -112(%ebp),%eax
	movl -64(%ebp),%edx
	movl $15,-136(%ebp)
	movl (%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	addl %esi,%edi
	movl -64(%ebp),%edx
	subl %edx,-136(%ebp)
	movl -112(%ebp),%eax
	movl -136(%ebp),%edx
	movl (%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	addl %esi,-140(%ebp)
	movl -100(%ebp),%eax
	movl -36(%ebp),%edx
	movl (%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	subl %esi,%edi
	movl $15,-136(%ebp)
	movl -36(%ebp),%edx
	subl %edx,-136(%ebp)
	movl -100(%ebp),%eax
	movl -136(%ebp),%edx
	movl (%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	addl %esi,-140(%ebp)
	movl -124(%ebp),%eax
	movl -68(%ebp),%edx
	movl $15,-136(%ebp)
	movl -640(%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	subl %esi,%edi
	movl -68(%ebp),%edx
	subl %edx,-136(%ebp)
	movl -124(%ebp),%eax
	movl -136(%ebp),%edx
	movl -640(%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	addl %esi,-140(%ebp)
	movl -124(%ebp),%eax
	movl -72(%ebp),%edx
	movl -512(%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	subl %esi,%edi
	movl $15,-136(%ebp)
	movl -72(%ebp),%edx
	subl %edx,-136(%ebp)
	movl -124(%ebp),%eax
	movl -136(%ebp),%edx
	movl -512(%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	addl %esi,-140(%ebp)
	movl -124(%ebp),%eax
	movl -76(%ebp),%edx
	movl $15,-136(%ebp)
	movl -384(%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	subl %esi,%edi
	movl -76(%ebp),%edx
	subl %edx,-136(%ebp)
	movl -124(%ebp),%eax
	movl -136(%ebp),%edx
	movl -384(%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	addl %esi,-140(%ebp)
	movl -124(%ebp),%eax
	movl -80(%ebp),%edx
	movl -256(%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	subl %esi,%edi
	movl $15,-136(%ebp)
	movl -80(%ebp),%edx
	subl %edx,-136(%ebp)
	movl -124(%ebp),%eax
	movl -136(%ebp),%edx
	movl -256(%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	addl %esi,-140(%ebp)
	movl -124(%ebp),%eax
	movl -84(%ebp),%edx
	movl $15,-136(%ebp)
	movl -128(%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	subl %esi,%edi
	movl -84(%ebp),%edx
	subl %edx,-136(%ebp)
	movl -124(%ebp),%eax
	movl -136(%ebp),%edx
	movl -128(%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	addl %esi,-140(%ebp)
	movl -124(%ebp),%eax
	movl -88(%ebp),%edx
	movl (%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	subl %esi,%edi
	movl $15,-136(%ebp)
	movl -88(%ebp),%edx
	subl %edx,-136(%ebp)
	movl -124(%ebp),%eax
	movl -136(%ebp),%edx
	movl (%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	addl %esi,-140(%ebp)
	movl -108(%ebp),%eax
	movl -92(%ebp),%edx
	movl $32,-136(%ebp)
	movl (%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%esi
	sall $16,%esi
	shrl $16,%ecx
	orl %ecx,%esi
	movl -108(%ebp),%eax
	movl -104(%ebp),%edx
	subl %esi,%edi
	movl (%eax),%eax
#APP
	imull (%ebx,%edx,4)
#NO_APP
	movl %edx,%ecx
	sall $16,%ecx
	shrl $16,%eax
	orl %eax,%ecx
	movl -20(%ebp),%edx
	movl -24(%ebp),%eax
	addl $64,%ebx
	movl %edi,(%eax,%edx,4)
	subl %edx,-136(%ebp)
	movl -136(%ebp),%edx
	addl -140(%ebp),%ecx
	movl %ecx,(%eax,%edx,4)
	addl $4,-128(%ebp)
	addl $4,-112(%ebp)
	addl $4,-96(%ebp)
	addl $4,-124(%ebp)
	addl $4,-108(%ebp)
	addl $4,-100(%ebp)
	incl -20(%ebp)
	cmpl $15,-20(%ebp)
	jbe .L28
	movl $D+1024,%esi
	movl -100(%ebp),%eax
	movl -36(%ebp),%edx
	movl (%eax),%eax
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ebx
	movl %edx,%ecx
	movl %ecx,%edi
	sall $16,%edi
	shrl $16,%ebx
	orl %ebx,%edi
	movl -100(%ebp),%eax
	movl -68(%ebp),%edx
	movl 128(%eax),%eax
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%ebx
	sall $16,%ebx
	shrl $16,%ecx
	orl %ecx,%ebx
	addl %ebx,%edi
	movl -100(%ebp),%eax
	movl -72(%ebp),%edx
	movl 256(%eax),%eax
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%ebx
	sall $16,%ebx
	shrl $16,%ecx
	orl %ecx,%ebx
	addl %ebx,%edi
	movl -100(%ebp),%eax
	movl -76(%ebp),%edx
	movl 384(%eax),%eax
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%ebx
	sall $16,%ebx
	shrl $16,%ecx
	orl %ecx,%ebx
	addl %ebx,%edi
	movl -100(%ebp),%eax
	movl -80(%ebp),%edx
	movl 512(%eax),%eax
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%ebx
	sall $16,%ebx
	shrl $16,%ecx
	orl %ecx,%ebx
	addl %ebx,%edi
	movl -100(%ebp),%eax
	movl -84(%ebp),%edx
	movl 640(%eax),%eax
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%ebx
	sall $16,%ebx
	shrl $16,%ecx
	orl %ecx,%ebx
	addl %ebx,%edi
	movl -100(%ebp),%eax
	movl -88(%ebp),%edx
	movl 768(%eax),%eax
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%ebx
	sall $16,%ebx
	shrl $16,%ecx
	orl %ecx,%ebx
	addl %ebx,%edi
	movl -100(%ebp),%eax
	movl -92(%ebp),%edx
	movl 896(%eax),%eax
#APP
	imull (%esi,%edx,4)
#NO_APP
	movl %eax,%ecx
	movl %edx,%ebx
	sall $16,%ebx
	shrl $16,%ecx
	orl %ecx,%ebx
	addl %ebx,%edi
	movl -24(%ebp),%edx
	negl %edi
	movl %edi,64(%edx)
	addl $4608,-116(%ebp)
	addl $4608,-120(%ebp)
	incl -12(%ebp)
	movl -4(%ebp),%eax
	cmpl %eax,-12(%ebp)
	jb .L22
.L17:
	incl -16(%ebp)
	movl -8(%ebp),%edx
	cmpl %edx,-16(%ebp)
	jb .L18
.L16:
	movl -8(%ebp),%edx
	salw $5,%dx
	movl 8(%ebp),%eax
	leal -152(%ebp),%esp
	movw %dx,4098(%eax)
	popl %ebx
	popl %esi
	popl %edi
	movl %ebp,%esp
	popl %ebp
	ret
.Lfe2:
	.size	 mad_synth_frame,.Lfe2-mad_synth_frame
	.align 16
.globl mad_synth_init
	.type	 mad_synth_init,@function
mad_synth_init:
	pushl %ebp
	movl %esp,%ebp
	pushl %esi
	xorl %edx,%edx
	pushl %ebx
	movl 8(%ebp),%esi
	leal 1024(%esi),%ebx
	leal 2048(%esi),%ecx
	leal 3072(%esi),%eax
	.p2align 4,,7
.L5:
	movl $0,(%eax)
	movl $0,(%ecx)
	movl $0,(%ebx)
	movl $0,(%esi,%edx,4)
	movl $0,4(%eax)
	movl $0,4(%ecx)
	movl $0,4(%ebx)
	movl $0,4(%esi,%edx,4)
	movl $0,8(%eax)
	movl $0,8(%ecx)
	movl $0,8(%ebx)
	movl $0,8(%esi,%edx,4)
	movl $0,12(%eax)
	movl $0,12(%ecx)
	movl $0,12(%ebx)
	movl $0,12(%esi,%edx,4)
	movl $0,16(%eax)
	movl $0,16(%ecx)
	movl $0,16(%ebx)
	movl $0,16(%esi,%edx,4)
	movl $0,20(%eax)
	movl $0,20(%ecx)
	movl $0,20(%ebx)
	movl $0,20(%esi,%edx,4)
	movl $0,24(%eax)
	movl $0,24(%ecx)
	movl $0,24(%ebx)
	movl $0,24(%esi,%edx,4)
	movl $0,28(%eax)
	movl $0,28(%ecx)
	movl $0,28(%ebx)
	movl $0,28(%esi,%edx,4)
	addl $32,%ebx
	addl $32,%ecx
	addl $32,%eax
	addl $8,%edx
	cmpl $255,%edx
	jbe .L5
	leal -8(%ebp),%esp
	movw $0,4096(%esi)
	popl %ebx
	popl %esi
	movl %ebp,%esp
	popl %ebp
	ret
.Lfe3:
	.size	 mad_synth_init,.Lfe3-mad_synth_init
	.ident	"GCC: (GNU) egcs-2.91.66 19990314/Linux (egcs-1.1.2 release)"
