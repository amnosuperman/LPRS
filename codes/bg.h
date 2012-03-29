/* 
 * Codes for background subtraction
 */
 
#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

# ifndef BG_H
# define BG_H

#define CHANNELS 3
typedef struct ce 
{
	uchar learnHigh[CHANNELS];
	uchar learnLow[CHANNELS];
	uchar max[CHANNELS];
	uchar min[CHANNELS];
	int t_last_update;
	int stale;
} code_element;

typedef struct code_book 
{
	code_element **cb;
	int numEntries;
	int t;
	//count every access
} codeBook;

int update_codebook(uchar* p,codeBook &c,int numChannels/*unsigned* cbBounds=10*/);
int clear_stale_entries(codeBook &c);
uchar background_diff(uchar* p,codeBook &c,int numChannels,int minMod,int maxMod);
void find_connected_components(IplImage *mask);
void startlearnbackground(CvCapture *capture,codeBook *c);
void updatebackground(CvCapture *capture,codeBook *c);
IplImage* diff(codeBook *c,IplImage *frame);

# endif
