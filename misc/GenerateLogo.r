# This is the R code for generating the logo
# Requires package "plot3D"

NX = 100
NY = 100

Spot1 = list (X=30, Y=40)
Spot2 = list (X=60, Y=70)

Z = matrix (nrow=NX+1, ncol=NY+1)
Height1 = 80
Height2 = 50
Rad1 = 25
Rad2 = 40

for (iX in 0:100){
	for (iY in 0:100){
		Z[iY+1, iX+1]= Height1 * exp(-4*log(2)*((iX-Spot1$X)^2+(iY-Spot1$Y)^2)/Rad1^2)+
					   Height2 * exp(-4*log(2)*((iX-Spot2$X)^2+(iY-Spot2$Y)^2)/Rad2^2)
	}
}

png ("E:\\Contour.png", width=5, height=5, units="in", res=600)
#Requires package "plot3D"
persp3D (x=0:NX, y=0:NY, z=Z, breaks = seq (min(Z), max(Z), length.out=101), col=hsv (seq(0.7,0, length.out=100),1,1),
	resfac=10,theta=55, phi=35, expand = 0.8)
dev.off()
write.table (Z,"E:\\Contour.txt",row.names=FALSE, col.names=FALSE, sep="\t")

