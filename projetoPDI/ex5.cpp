/**
* TUTORIAL OPENCV
* EXEMPLO 5: Exibir um vídeo à partir de uma câmera ou arquivo
*
* @author   MALF - malf@tecgraf.puc-rio.br
* @date     29/06/07
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
//#include "cvcam.h"
#include <unistd.h>
#include "cv.h"
#include "highgui.h"
/************************************************************************
 *
 ************************************************************************/
 void videoAnalize( int, void* );
int main( int argc, char** argv )
{
  IplImage* frame = 0, *background, *foreground, *framecinza, *clone, *clone2;
  CvCapture* capture = 0;

  char cwd[1024];

  CvPoint min_point, max_point;
  int max_x, max_y, min_x, min_y, threshold_value = 70, threshold_type = 1, bounding_box = 30, approximation_type = 1;
  //int pix;

  // capture = cvCaptureFromCAM(0);

  //capture = cvCaptureFromAVI( "c:\\v4.mpeg" );
  getcwd(cwd, 1024);
  strcat(cwd, "\\v4.avi");

  capture = cvCaptureFromFile(cwd);

  if( !capture )
  {
    fprintf(stderr,"Could not initialize capturing...\n");
     system("pause");
  }

  frame = cvQueryFrame( capture );
  frame->origin = 0;

  background = cvCreateImage(cvSize(frame->width, frame->height),frame->depth,1);
  framecinza = cvCreateImage(cvSize(frame->width, frame->height),frame->depth,1);
  background->origin = 0;
  cvCvtColor(frame,background,CV_RGB2GRAY);

  foreground = cvCreateImage(cvSize(frame->width, frame->height),frame->depth,1);
  foreground->origin = 0;



  cvNamedWindow( "in", 1);
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
  clone = cvCloneImage(foreground);

CvMemStorage* storage = cvCreateMemStorage();
	CvSeq* first_contour = NULL;

    int Nc = cvFindContours(
        clone,
		 storage,
		 &first_contour,
		 sizeof(CvContour),
		 CV_RETR_LIST, approximation_type+1);

	 //printf( "Total de contornos: %d\n", (int)first_contour.total );
	CvScalar red = CV_RGB(255, 0, 0);

	int n=0;

	for( CvSeq* c=first_contour; c!=NULL; c=c->h_next ){
        min_x = 1829182;
        min_y = 1290291;
        max_x = -1;
        max_y = -1;

		// Recupera a coordenada de cada ponto
        for( int i=0; i<c->total; i++ ){
			CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, c, i );
			//  printf(" (%d,%d)\n", p->x, p->y );
//			if(c->h > 100){
			if(p->x < min_x){
                min_x = p->x;
			}
			if(p->y < min_y){
                min_y = p->y;
			}
			if(p->x > max_x){
                max_x = p->x;
			}
			if(p->y > max_y){
                max_y = p->y;
			}
			//}
		}
        if(max_x-min_x < bounding_box && max_y-min_y < bounding_box){
            min_x = 1829182;
            min_y = 1290291;
            max_x = -1;
            max_y = -1;
        }else{
            min_point.x = min_x;
            min_point.y = min_y;
            max_point.x = max_x;
            max_point.y = max_y;
        }
		//clone2 = cvCloneImage(clone);
		n++;
		cvRectangle(frame,min_point,max_point,red);
        //printf("Min: (%d,%d) | Max: (%d,%d)\n", min_x, min_y, max_x, max_y );
	}



     cvErode(foreground, foreground, NULL, 1);
     cvDilate( foreground,foreground,NULL, 2);

/*
    for(int i=0;i<background->height;i++)
              for(int j=0;j<background->width;j++) {
               ((uchar*)(background->imageData + background->widthStep*i))[j] = 255;
    }
*/
    cvShowImage("in", frame);
    cvShowImage("fore", foreground );
    cvShowImage("clone", clone );
    cvCreateTrackbar(" Threshold: ", "fore", &threshold_value, 255, NULL);
    cvCreateTrackbar(" Type: ", "fore", &threshold_type, 4, NULL);
     cvCreateTrackbar("Approximation: ", "clone", &approximation_type, 3, NULL  );
    cvCreateTrackbar(" BB: ", "in", &bounding_box, 100, NULL);
   if( cvWaitKey(10) >= 0 )
        break;
  }
  cvReleaseCapture( &capture );
  cvDestroyAllWindows();
  system("pause");
  return 0;
}
