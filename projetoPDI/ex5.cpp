/**
* TUTORIAL OPENCV
* EXEMPLO 5: Exibir um v�deo � partir de uma c�mera ou arquivo
*
* @author   MALF - malf@tecgraf.puc-rio.br
* @date     29/06/07
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <unistd.h>
#include <math.h>
#include "cv.h"
#include "highgui.h"
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))
/************************************************************************
 *
 ************************************************************************/
 int usados[1500], contagem = 0;

 void videoAnalize( int, void* );

 void zerarAtual(CvPoint atual[], CvPoint anterior[]){
     int i;
     contagem=0;
     for(i=0;i<100;i++){
        anterior[i]=atual[i];
        atual[i].x = 0;
        atual[i].y = 0;
    }
 }
 void zerarAnterior(CvPoint anterior[]){
    for(int i=0;i<100;i++){
        anterior[i].x = 0;
        anterior[i].y = 0;
    }
 }

int distancia_euclidiana(CvPoint atual, CvPoint anterior[]){
    int i,j;
    double distancia, aux=10000;
    for(i=0;i<100;i++){
        distancia = sqrt(pow((atual.x-anterior[i].x),2) + pow((atual.y-anterior[i].y),2));
        //aux = min(distancia, aux);
           if(distancia<aux && anterior[i].x>0 && anterior[i].y>0 /*&& distancia<50*/){
                aux = distancia;
                j=i;
            }


        //    printf("%.2f // %.2f\n", aux, distancia);
        //printf("%d -> %d x %d -> %d x %d\n", i, atual.x, anterior[i].x, atual.y, anterior[i].y);
    }
    return j;
}

int main( )
{

  IplImage* frame = 0, *background, *foreground, *framecinza, *cnt_img, *clone;
  CvCapture* capture = 0;

  char cwd[1024];

/*  rotulo[1] = "rotulo1";
  rotulo[2] = "rotulo2";
  rotulo[3] = "rotulo3";
  rotulo[4] = "rotulo4";*/
  CvPoint min_point, max_point, center_point, centroides_frameatual[100], centroides_frameanterior[100];
  int max_x, max_y, min_x, min_y, threshold_value = 70, threshold_type = 1, bounding_box = 30, approximation_type = 1, j;
  //int pix;hh

  // capture = cvCaptureFromCAM(0);

  //capture = cvCaptureFromAVI( "c:\\v4.mpeg" );
  getcwd(cwd, 1024);
  strcat(cwd, "\\v4.avi");

  capture = cvCaptureFromFile(cwd);

  if( !capture )
  {
    printf("Could not initialize capturing...\n");
    // system("pause");
  }

  frame = cvQueryFrame( capture );
  frame->origin = 0;

  background = cvCreateImage(cvSize(frame->width, frame->height),frame->depth,1);
  framecinza = cvCreateImage(cvSize(frame->width, frame->height),frame->depth,1);
  background->origin = 0;
  cvCvtColor(frame,background,CV_RGB2GRAY);

  foreground = cvCreateImage(cvSize(frame->width, frame->height),frame->depth,1);
  foreground->origin = 0;



  cvNamedWindow( "in",1);
  cvNamedWindow( "fore", 1);
   cvNamedWindow( "clone", 1);


  for(;;)
  {
    if(!cvGrabFrame(capture))
      break;

    frame = cvRetrieveFrame(capture);
    cvCvtColor(frame,framecinza,CV_RGB2GRAY);

    cvAbsDiff(framecinza,background,foreground);
    cvThreshold(foreground, foreground,threshold_value,255,threshold_type);
    //cvSub(foreground, foreground, foreground,0);
    cvErode(foreground, foreground, NULL, 2);
    cvDilate(foreground,foreground,NULL, 2);

    clone = cvCloneImage(foreground);

    CvMemStorage* storage = cvCreateMemStorage();
	CvSeq* first_contour = 0;

    cvFindContours(
         clone,
		 storage,
		 &first_contour,
		 sizeof(CvContour),
		 CV_RETR_LIST, approximation_type+1);
	CvSeq* _contours = first_contour;
    cnt_img = cvCreateImage( cvSize(frame->width,frame->height), 8, 3 );
    cvZero( cnt_img );
    cvDrawContours( cnt_img, _contours, CV_RGB(255,0,0), CV_RGB(0,255,0), 4, 2, 8);
	 //printf( "Total de contornos: %d\n", (int)first_contour.total );
	CvScalar red = CV_RGB(190, 5, 190);

	int n=0;
    j = 0;
    //>centroides_frameanterior = centroides_frameatual; > Criar fun��o para colocar no frameanterior o que t� no frame atual e zerar o frame atual com \0 para poder receber os novos centroides
    zerarAtual(centroides_frameatual, centroides_frameanterior);

	for( CvSeq* c=first_contour; c!=NULL; c=c->h_next ){
        min_x = 1829182;
        min_y = 1290291;
        max_x = -1;
        max_y = -1;

        /*if(c->total < 10){
            contagem++;
        }else{
            contagem = 0;
        }*/

		// Recupera a coordenada de cada ponto
        for( int i=0; i<c->total; i++ ){
			CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, c, i );

			 //cvApproxPoly(CvSeq, sizeof(CvContour), storage,CV_POLY_APPROX_DP, true);
			//  printf(" (%d,%d)\n", p->x, p->y );
//			if(c->h > 100){

            min_x = min(min_x, p->x);
            min_y = min(min_y, p->y);
            max_x = max(max_x, p->x);
            max_y = max(max_y, p->y);

		}
        if((max_x-min_x >= bounding_box || max_y-min_y >= bounding_box) && max_x-min_x != frame->width-3 && max_y-min_y != frame->height-3){
            contagem++;
            min_point.x = min_x;
            min_point.y = min_y;
            max_point.x = max_x;
            max_point.y = max_y;

            center_point.x = (int)(min_point.x+(max_point.x-min_point.x)/2);
            center_point.y = (int)(min_point.y+(max_point.y-min_point.y)/2);
            int distancia;
            char buff[5];


            distancia = distancia_euclidiana(cvPoint(center_point.x, center_point.y), centroides_frameanterior);
            itoa(contagem, buff,5);
            if(distancia>0){
                    j++;
                 centroides_frameatual[distancia] = center_point;
            }else {
                 centroides_frameatual[j] = center_point;
            }
            CvFont font;
            cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1, 1);
            cvPutText(cnt_img, buff, center_point, &font, CV_RGB(255,255,255));
            cvLine(frame,cvPoint(center_point.x-4, center_point.y), cvPoint(center_point.x+4, center_point.y), CV_RGB(128,128,0),1);
            cvLine(frame,cvPoint(center_point.x, center_point.y-4), cvPoint(center_point.x, center_point.y+4), CV_RGB(128,128,0),1);
            cvRectangle(frame,min_point,max_point,red,2);
           // printf("%d -> %d  x %d -> %d\n", j, centroides_frameatual[j].x, centroides_frameatual[j].y, distancia);
           j++;
        }
        //printf("%d %d\n", center_point.x, center_point.y);
        /*if(contagem == 10){
            zerarAnterior(centroides_frameanterior);
            j++;
        }*/
		//clone2 = cvCloneImage(clone);
		n++;
        //printf("Min: (%d,%d) | Max: (%d,%d)\n", min_x, min_y, max_x, max_y );
     //   cvWaitKey(10);
	}





    cvShowImage("in", frame);
    cvShowImage("clone", cnt_img );
    cvShowImage("fore", foreground );
    cvCreateTrackbar(" Threshold: ", "fore", &threshold_value, 255, NULL);
    cvCreateTrackbar(" Type: ", "fore", &threshold_type, 4, NULL);
     cvCreateTrackbar("Approximation: ", "clone", &approximation_type, 3, NULL  );
    cvCreateTrackbar(" BB: ", "in", &bounding_box, 100, NULL);
   if( cvWaitKey(10) >= 0 )
        break;
  }
  cvReleaseCapture( &capture );
  //cvDestroyAllWindows();
  //system("pause");
  return 0;
}


