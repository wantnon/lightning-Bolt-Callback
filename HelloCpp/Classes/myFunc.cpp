//
//  myFunc.cpp
//  HelloCpp
//
//  Created by apple on 13-10-26.
//
//

#include "myFunc.h"
#include "support/ccUtils.cpp"//if we use #include "support/ccUtils.cpp", in vs2010 cause linking error "unsolved external symbol".

CCTexture2D* createCCTexture2DWithSize(const CCSize&size,CCTexture2DPixelFormat eFormat,
                                       float r=0,float g=0,float b=0,float a=0)//r,g,b,a are range form 0 to 1
{
    //the code below is copied from CCRenderTexture::initWithWidthAndHeight, with some modification.
    CCTexture2D* pTexture=NULL;
    void *data = NULL;
    do{
        int w = (int)(size.width * CC_CONTENT_SCALE_FACTOR());
        int h = (int)(size.height * CC_CONTENT_SCALE_FACTOR());
        
        // textures must be power of two squared
        unsigned int powW = 0;
        unsigned int powH = 0;
        
        if (CCConfiguration::sharedConfiguration()->supportsNPOT())
        {
            powW = w;
            powH = h;
        }
        else
        {
            powW = ccNextPOT(w);
            powH = ccNextPOT(h);
        }
        
        data = malloc((int)(powW * powH * 4));
        CC_BREAK_IF(! data);
        
        memset(data, 0, (int)(powW * powH * 4));
        
        //fill data with color value
        int _r=r*255;
        int _g=g*255;
        int _b=b*255;
        int _a=a*255;
        for(int i=0;i<powH;i++){
            for(int j=0;j<powW;j++){
                ((char*)data)[(i*powW+j)*4+0]=_r;
                ((char*)data)[(i*powW+j)*4+1]=_g;
                ((char*)data)[(i*powW+j)*4+2]=_b;
                ((char*)data)[(i*powW+j)*4+3]=_a;
            }
        }
        
        pTexture = new CCTexture2D();
        if (pTexture)
        {
            pTexture->initWithData(data, eFormat, powW, powH, CCSizeMake((float)w, (float)h));
        }
    }while (0);
    return pTexture;
}
float random01(){
    return rand()*1.0/RAND_MAX;
}
float randomInRange(float min,float max){
    return min+random01()*(max-min);
}
CCPoint rotateVec(const CCPoint&vec,float A){//A is degree
    CCPoint rs;
    float A_radian=A*M_PI/180;
    float cosA=cosf(A_radian);
    float sinA=sinf(A_radian);
    rs.x=vec.x*cosA-vec.y*sinA;
    rs.y=vec.x*sinA+vec.y*cosA;
    return rs;
}
CCPoint rotateVec(const CCPoint&vec,float cosA,float sinA){
    CCPoint rs;
    rs.x=vec.x*cosA-vec.y*sinA;
    rs.y=vec.x*sinA+vec.y*cosA;
    return rs;
}
bool catchProbability(float p)
//the probability of return true is p
{
    float r=random01();
    if(r<p)return true;
    else return false;

}
float calAngleFromCosAndSin(float cosA,float sinA){
    float angle=acosf(cosA)/M_PI*180;
    if(sinA>0){
        return angle;
    }else{
        return 360-angle;
    }
    
}
bool pointEqual(const CCPoint&point1,const CCPoint&point2,float eps){
    if(fabsf(point1.x-point2.x)<eps&&fabsf(point1.y-point2.y)<eps){
        return true;
    }else{
        return false;
    }
}
void cross(const float v1[3],const float v2[3], float rs[3])
{
    rs[0] = v1[1]*v2[2] - v1[2]*v2[1];
    rs[1] = v1[2]*v2[0] - v1[0]*v2[2];
    rs[2] = v1[0]*v2[1] - v1[1]*v2[0];
}
CCPoint calculateAngleFromVec1ToVec2(const CCPoint&vec1,const CCPoint&vec2)
//return {cos(vec1,vec2),sin(vec1,vec2)}
{
    float cosA=ccpDot(vec1, vec2)/(ccpLength(vec1)*ccpLength(vec2));
    float signalOfSinA;
    {
        float _vec1[3]={vec1.x,vec1.y,0};
        float _vec2[3]={vec2.x,vec2.y,0};
        float _rs[3];
        cross(_vec1, _vec2, _rs);
        if (_rs[2]==0) {
            signalOfSinA=0;
        }else if(_rs[2]>0){
            signalOfSinA=1;
        }else{
            signalOfSinA=-1;
        }
    }
    float sinA=signalOfSinA*sqrtf(MAX(0,1-cosA*cosA));
    return CCPoint(cosA,sinA);
}
struct _node{
    _node*foe;
    bool value;
};
float nPow(float a,int n){
    assert(!(a==0&&n==0));//0^0 nonsense
    if(n==0&&a!=0)return 1;
    if(a==0&&n!=0)return 0;
    if(n==1)return a;
    if(a==1)return 1;
    _node*tail=NULL;
    while(n!=1){
        _node*node=new _node();
        node->value=n%2;
        node->foe=tail;
        tail=node;
        n=n>>1;
    }//got a chain use tail as tail pointer
    float rs=a;
    while(tail){
        if(tail->value){
            rs=rs*rs*a;
        }else{
            rs=rs*rs;
        }
        _node*temp=tail;
        tail=tail->foe;
        delete temp;
    }
    return rs;
    
}
