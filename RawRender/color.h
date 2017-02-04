#ifndef	_COLOR_H_
#define	_COLOR_H_


#ifdef __cplusplus
extern "C" {
#endif

typedef struct color_impl
{
	double red;
	double green;
	double blue;
}
Color;

Color colorCreate3b( unsigned char red, unsigned char green, unsigned char blue );

Color colorAddition( Color c1, Color c2 );

Color colorScale( double scale, Color color );

Color colorMultiplication( Color c1, Color c2 );

Color colorReflection( double scale, Color light, Color material );

Color colorNormalize( Color color );

#ifdef __cplusplus
}
#endif

#endif

