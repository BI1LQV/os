import cv2
cap = cv2.VideoCapture(0,cv2.CAP_DSHOW)
def process_frame(frame):
    return frame

def get_img():

    success, frame = cap.read()
    success, frame2 = cap.read()
    if not success:
        print('error!')
    #frame = process_frame(frame)

    cv2.destroyAllWindows()
    gray=cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
    rst=[]
    for i in gray:
        for j in i:
            rst.append(j)
    gray2=cv2.cvtColor(frame2,cv2.COLOR_BGR2GRAY)
    rst2=[]
    for i in gray2:
        for j in i:
            rst2.append(j)
    return rst,rst2
def main():
    while(1):
        rst1,rst2=pixel1=get_img()
        #rst2=pixel2=get_img()
        resu=[]
        for i in range(len(rst1)):
            if(rst1[i]!=rst2[i]):
                resu.append(abs(rst1[i]-rst2[i])%2)
        print("本轮生成二进制随机数位数:",len(resu))
        output=""
        for i in resu[:200]:
            output=output+str(i)
        print("随机数前200位为:",output)
    cap.release()
if __name__ == '__main__':
    main()