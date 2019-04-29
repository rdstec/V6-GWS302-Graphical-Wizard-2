// Graphic Wizard Display Routines

// Include Standard files
#include "global.h"

// functions
/*-----------------------------------------------------------*/
void setPixel( unsigned char *buff, unsigned int x, unsigned int y, unsigned int method )
{
	register unsigned char *pt, n;
	unsigned int offset;

	offset = (x / 8) + (y * COLUMNS);
	if( offset < GRAPHICS_SIZE )
	{
		n = (unsigned char)0x80 >> (x % 8);
		pt = &buff[offset];
		if( method == CLEAR )
		{
			*pt &= ~n;
		}
		else
		{
			*pt |= n;
		}
	}
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void circlePoints( unsigned char *buff, unsigned int cx, unsigned int cy, unsigned int x, unsigned int y, unsigned int method )
{

	if( x == 0 )
	{
		setPixel( buff, cx, cy + y, method );
		setPixel( buff, cx, cy - y, method );
		setPixel( buff, cx + y, cy, method );
		setPixel( buff, cx - y, cy, method );
	}
	else if( x == y )
	{
		setPixel( buff, cx + x, cy + y, method );
		setPixel( buff, cx - x, cy + y, method );
		setPixel( buff, cx + x, cy - y, method );
		setPixel( buff, cx - x, cy - y, method );
	}
	else if( x < y )
	{
		setPixel( buff, cx + x, cy + y, method );
		setPixel( buff, cx - x, cy + y, method );
		setPixel( buff, cx + x, cy - y, method );
		setPixel( buff, cx - x, cy - y, method );
		setPixel( buff, cx + y, cy + x, method );
		setPixel( buff, cx - y, cy + x, method );
		setPixel( buff, cx + y, cy - x, method );
		setPixel( buff, cx - y, cy - x, method );
	}
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void circleMidpoint( unsigned char *buff, unsigned int xCenter, unsigned int yCenter, unsigned int radius, unsigned int method )
{
	unsigned int x = 0;
	unsigned int y = radius;
	signed int p = (5 - ((signed int)radius * 4)) / 4;

	circlePoints( buff, xCenter, yCenter, x, y, method );
	while( x < y )
	{
		x++;
		if( p < 0 )
		{
			p += ((2 * x) + 1);
		}
		else
		{
			y--;
			p += ((2*(x-y)) + 1);
		}
		circlePoints( buff, xCenter, yCenter, x, y, method );
	}
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
int greatest( int *a, int *b )
{
	register unsigned char c;

	if( *a < *b )
	{
		c = *b;
		*b = *a;
		*a = c;
	}

	return( *a - *b );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void draw_line( unsigned char *buff, unsigned int xp1, unsigned int yp1, unsigned int xp2, unsigned int yp2, unsigned int method )
{
	unsigned int x, y, dx, dy, offset, dir;
	register unsigned char *pt, n;

	if( xp1 > 159 )
	{
		xp1 = 159;
	}
	if( xp2 > 159 )
	{
		xp2 = 159;
	}
	if( yp1 > 127 )
	{
		yp1 = 127;
	}
	if( yp2 > 127 )
	{
		yp2 = 127;
	}

	if ( ((xp2 >= xp1) && (yp2 >= yp1)) || ((xp2 < xp1) && (yp2 < yp1)) )
	{
		dir = 0;
	}
	else
	{
		dir = 1;
	}

	dx = greatest( &xp2, &xp1 );
	dy = greatest( &yp2, &yp1 );

	if( dir )
	{
		x = xp2;
		for( y = 0; y <= dy; y++ )
		{
			offset = (x / 8) + ((y + yp1) * COLUMNS);
			n = (unsigned char)(0x80 >> (x % 8));
			while( ((y * dx) >= ((xp2 - x) * dy)) && (x >= xp1) )
			{
				pt = &buff[offset];
				if( method == CLEAR )
				{
					*pt &= ~n;
				}
				else
				{
					*pt |= n;
				}
				if( !(n <<= 1) )
				{
					n = 0x01;
					offset--;
				}
				if( x )
				{
					x--;
				}
				else
				{
					break;
				}
			}
			x++;
		}
	}
	else
	{
		x = xp1;
		for( y = 0; y <= dy; y++ )
		{
			offset = (x / 8) + ((y + yp1) * COLUMNS);
			n = (unsigned char)0x01 << (7 - (x % 8));
			while( ((y * dx) >= ((x - xp1) * dy)) && (x <= xp2) )
			{
				if( method == CLEAR )
				{
					buff[offset] &= ~n;
				}
				else
				{
					buff[offset] |= n;
				}
				if( !(n >>= 1) )
				{
					n = 0x80;
					offset++;
				}
				x++;
			}
			x--;
		}
	}

}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void draw_box( unsigned char *buff, unsigned int xp1, unsigned int yp1, unsigned int xp2, unsigned int yp2, unsigned int method )
{
	/* GREY, CLEAR, FULL, INV or EMPTY(default), */
	unsigned int start_byte, end_byte, right_shift, left_shift, dx, bytes;
	unsigned int offset, point, x, y;
	register unsigned char n, c, cc, nn, mask;

	if( xp1 > 159 )
	{
		xp1 = 159;
	}
	if( xp2 > 159 )
	{
		xp2 = 159;
	}
	if( yp1 > 127 )
	{
		yp1 = 127;
	}
	if( yp2 > 127 )
	{
		yp2 = 127;
	}

	dx = greatest( &xp2, &xp1 );
	greatest( &yp2, &yp1 );

	if( !dx )
	{
		draw_line( buff, xp1, yp1, xp1, yp2, method );
		return;
	}
	else if( dx == 1)
	{
		draw_line( buff, xp1, yp1, xp1, yp2, method );
		draw_line( buff, xp2, yp1, xp2, yp2, method );
		return;
	}

	start_byte = xp1 / 8;
	right_shift = xp1 % 8;
	end_byte = xp2 / 8;
	left_shift = 7-(xp2 % 8);
	bytes = end_byte - start_byte;

	c = (unsigned char)0xff >> right_shift;
	n = (unsigned char)0xff << left_shift;

	switch ( method )
	{
	case GREY:
		for( y = yp1; y <= yp2; y++ )
		{
			mask = (y & 1) ? 0xaa : 0x55;
			cc = c & mask;
			nn = n & mask;

			offset = start_byte + (y * COLUMNS);
			if( !bytes )
			{
				buff[offset++] &= ((cc & nn) | ~(c & n));
			}
			else
			{
				buff[offset++] &= (cc | ~c);
				for( x = start_byte+1; x < end_byte; x++ )
				{
					buff[offset++] &= mask;
				}
				buff[offset] &= (nn | ~n);
			}
		}
		break;

	case CLEAR:
		for( y = yp1; y <= yp2; y++ )
		{
			offset = start_byte + (y * COLUMNS);
			if( !bytes )
			{
				buff[offset++] &= ~(c & n);
			}
			else
			{
				buff[offset++] &= ~c;
				for( x = start_byte+1; x < end_byte; x++ )
				{
					buff[offset++] = 0x00;
				}
				buff[offset] &= ~n;
			}
		}
		break;

	case FULL:
		for( y = yp1; y <= yp2; y++ )
		{
			offset = start_byte + (y * COLUMNS);
			if( !bytes )
			{
				buff[offset++] |= (c & n);
			}
			else
			{
				buff[offset++] |= c;
				for( x = start_byte+1; x < end_byte; x++ )
				{
					buff[offset++] |= 0xff;
				}
				buff[offset] |= n;
			}
		}
		break;

	case INV:
	case INVCLP:
		for( y = yp1; y <= yp2; y++ )
		{
			offset = start_byte + (y * COLUMNS);
			if( !bytes )
			{
				//buff[offset] &= ~(c & n);
				//buff[offset] |= ~c ;
				//buff[offset++] |= ~n ;
				buff[offset] ^= (c & n);
			}
			else
			{
				//cc = ~buff[offset];
				//cc &= c;
				//buff[offset] &= ~c;
				//buff[offset] |= cc;
				buff[offset] ^= c;

				offset++;

				for( x = start_byte + 1; x < end_byte; x++ )
				{
					buff[offset] ^= 0xff;
					offset++;
				}

				//nn = ~buff[offset];
				//nn &= n;
				//buff[offset] &= ~n;
				//buff[offset] |= nn;
				buff[offset] ^= n;

				offset++;
			}
		}
		break;

	default: /* EMPTY */
		offset = start_byte + (yp1 * COLUMNS);
		point =  start_byte + (yp2 * COLUMNS);

		if( !bytes )
		{
			c = c & n;
			buff[offset++] |= c;
			buff[point++] |= c;
		}
		else
		{
			buff[offset++] |= c;
			buff[point++] |= c;

			for( x = start_byte+1; x < end_byte; x++ )
			{
				buff[offset++] |= 0xff;
				buff[point++] |= 0xff;
			}
			buff[offset] |= n;
			buff[point] |= n;
		}

		c = (unsigned char)0x80 >> right_shift;
		n = (unsigned char)0x01 << left_shift;

		for( y = yp1+1; y < yp2; y++ )
		{
			offset = start_byte + (y * COLUMNS);
			point =  end_byte + (y * COLUMNS);
			buff[offset] |= c;
			buff[point] |= n;
		}
		break;
	}

	if( method == INVCLP )
	{ /* clip the corners off */
		draw_line( adm, xp1, yp1, xp1, yp1, CLEAR );
		draw_line( adm, xp2, yp1, xp2, yp1, CLEAR );
		draw_line( adm, xp1, yp2, xp1, yp2, CLEAR );
		draw_line( adm, xp2, yp2, xp2, yp2, CLEAR );
	}
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
// write_bitmap
// ptd - a buffer to write raster scan for display driver
// pts - uncompressed bitmap array
// pts[0] = bit width of bit map
// pts[1] = bit height of bit map
// xpos,ypos - x,y location on display, top left = 0,0
// method AND, OR, INV
/*-----------------------------------------------------------*/
unsigned char write_bitmap( unsigned char *ptd, const unsigned char *pts, unsigned int xpos, unsigned int ypos, unsigned int method )
{
	unsigned int j, width, x, y, n, c, d;
	unsigned char *lptd, *lpts;
	register unsigned char imL, imR;

	if( method & INV )
	{
		imL = (unsigned char)0xff;
		imR = (unsigned char)0xff << (8-(pts[0] % 8));
	}
	else
	{
		imL = 0;
		imR = 0;
	}

	c = xpos % 8; /* xpos adjust, right shift bitmap */
	d = 8 - c;
	width = (pts[0] + 7) / 8;

	lpts = (unsigned char *)&pts[2];
	lptd = &buffer[0];
	for( y = 0; y < pts[1]; y++ )
	{
		*lptd++ = ((*lpts ^ imL) >> c);
		for( x = 1; x < width; x++ )
		{
			*lptd = ((*lpts ^ imL) << d);
			if( x == (width - 1) )
			{
				*lptd++ |= ((*(++lpts) ^ imR) >> c);
			}
			else
			{
				*lptd++ |= ((*(++lpts) ^ imL) >> c);
			}
		}
		*lptd++ = ((*lpts++ ^ imR) << d);
	}

	width++;

	x = (xpos / 8);
	lpts = &buffer[0];
	for( n = 0; n < pts[1]; n++ )
	{
		j = x + ((ypos + n) * COLUMNS);
		lptd = &ptd[j];
		for( c = 0; (c < width) && (j < GRAPHICS_SIZE); c++ )
		{
			if( (method & OR) == OR )
			{
				*lptd++ |= *lpts++;
			}
			else if ( (method & XOR) == XOR )
			{
				*lptd++ ^= *lpts++;
			}
			else  /*  default = AND */
			{
				*lptd++ = *lpts++;
			}
			j++;  
		}
	}

	return( pts[0] );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
unsigned char display_f1_text( const unsigned char *data, unsigned int max, unsigned int co, unsigned int ro, unsigned int prog )
{
	static int pos;
	static unsigned char dstring[32];
	
	unsigned int i, loc, wid, lco, coo, chr;
	unsigned char *pt;
	struct fn1 const *font;

	font = fnt1[pFac.pf.Lang];
	
	if( prog & EDIT )
	{
		if( editing )
		{
			if( key & (SW_NEW | SW_REP) )
			{
				key &= ~(SW_NEW | SW_REP); // mask off status bits
				if( key == SW_OR )
				{
					switch( editing )
					{
					case EDIT_UPPER:	
						editing = EDIT_LOWER; 
						break;

					case EDIT_LOWER:
						editing = EDIT_NUMERIC; 
						break;

					case EDIT_NUMERIC:
						editing = EDIT_UPPER; 
						break;
					}
				}
				else if( key == SW_IR )
				{
					editing = 0;
					strcpy( (char *)data, dstring );
					// if factor is in pFac address space then save to flash 
					if( ((unsigned char *)data >= (unsigned char *)&pFac) &&
						((unsigned char *)data <  (unsigned char *)(&pFac + sizeof(struct PreservedFactors))) )
					{
						savecal = 1;
					}

					if( prog & EDSCAPE )
					{
						escape = 3;
					}
				}
				else if( key == SW_CR )
				{
					if( pos < (max - 1) )
					{
						pos++;
					}
				}
				else if( key == SW_CL )
				{
					if( pos )
					{
						pos--;
					}
				}
				else if( key == SW_IL )
				{
					editing = 0;
					if( prog & EDSCAPE )
					{
						escape = 2;
					}
				}
				else if( (key == SW_CU) || (key == SW_CD) )
				{
					chr = dstring[pos];
					i = 0;
					// find match in upper case data
					while( UCkey[i] && (UCkey[i] != chr) )
					{
						i++;
					}
					//  match
					if( UCkey[i] )
					{
						switch( editing )
						{
						case EDIT_UPPER:	
							pt = (unsigned char *)&UCkey[0];
							loc = i;
							break;

						case EDIT_LOWER:
							pt = (unsigned char *)&LCkey[0];
							loc = i;
							break;

						case EDIT_NUMERIC:
							pt = (unsigned char *)&NCkey[0];
							loc = 0;
							break;
						}
					}
					else // no match
					{
						i = 0;
						// find match in lower case data
						while( LCkey[i] && (LCkey[i] != chr) )
						{
							i++;
						}
						if( LCkey[i] )
						{
							switch( editing )
							{
							case EDIT_UPPER:	
								pt = (unsigned char *)&UCkey[0];
								loc = i;
								break;

							case EDIT_LOWER:
								pt = (unsigned char *)&LCkey[0];
								loc = i;
								break;

							case EDIT_NUMERIC:
								pt = (unsigned char *)&NCkey[0];
								loc = 0;
								break;
							}
						}
						else // no match
						{
							i = 0;
							// find match in numeric data
							while( NCkey[i] && (NCkey[i] != chr) )
							{
								i++;
							}
							if( NCkey[i] )
							{
								switch( editing )
								{
								case EDIT_UPPER:	
									pt = (unsigned char *)&UCkey[0];
									loc = 0;
									break;

								case EDIT_LOWER:
									pt = (unsigned char *)&LCkey[0];
									loc = 0;
									break;

								case EDIT_NUMERIC:
									pt = (unsigned char *)&NCkey[0];
									loc = i;
									break;
								}
							}
							else // no match
							{
								switch( editing )
								{
								case EDIT_UPPER:	
									pt = (unsigned char *)&UCkey[0];
									loc = 0;
									break;

								case EDIT_LOWER:
									pt = (unsigned char *)&LCkey[0];
									loc = 0;
									break;

								case EDIT_NUMERIC:
									pt = (unsigned char *)&NCkey[0];
									loc = 0;
									break;
								}
							}
						}
					}

					if( key == SW_CU )
					{
						// set next character or first	
						loc++;
						if( *(pt + loc) )
						{
							dstring[pos] = *(pt + loc);
						}
						else
						{
							dstring[pos] = *pt;
						}

					}
					else // key == SW_CD
					{
						// set previous character or last					
						if( loc )
						{
							loc--;
							dstring[pos] = *(pt + loc);
						}
						else
						{
							while( *pt )
							{
								pt++;
							}
							pt--;
							dstring[pos] = *pt;
						}
					}

				}
			}		
		}
		else if( (prog & EDSCAPE) || (key == (SW_NEW | SW_CR)) )
		{
			editing = EDIT_UPPER;
			pos = 0;
			looptime = 0;
			strcpy( dstring, data );
			i = max;
			dstring[i--] = 0x00;
			while( (dstring[i] == 0x00) && i )
			{
				dstring[i--] = ' ';
			}
		}
	}

	wid = 0;
	if( (prog & EDIT) && editing )
	{
		for( i=0; (chr = dstring[i]) && (i < max); i++ )
		{
			wid += (font->f1[chr][0] + font->spc);
		}
	}
	else
	{
		for( i=0; (chr = *(data+i)) && (i < max); i++ )
		{
			wid += (font->f1[chr][0] + font->spc);
		}
	}
	wid = wid - font->spc - 1;

	if( prog & CENTER )
	{
		co = co - (wid / 2);
	}
	wid += co;

	if( (prog & EDIT) && editing )
	{
		draw_box( adm, co-1, ro-1, wid+1, ro+8, CLEAR );
	}
	else
	{
		draw_box( adm, co-1, ro-1, wid+1, ro+7, CLEAR );
	}
	
	if( prog & INV )
	{
		draw_box( adm, co-1, ro-1, wid+1, ro+8, EMPTY );
	}

	coo = co;

	if( (prog & EDIT) && editing )
	{
		for( i=0; (chr = dstring[i]) && (i < max); i++ )
		{
			lco = co;
			co += (write_bitmap( adm, font->f1[chr], co, ro, OR ) + font->spc);
			if( (i == pos) && !half_second )
			{
				draw_box( adm, lco-1, ro-1, co-1, ro+8, INV );
			}
		}
	}
	else
	{
		for( i=0; (chr = *(data+i)) && (i < max); i++ )
		{
			co += (write_bitmap( adm, font->f1[chr], co, ro, OR ) + font->spc);
		}
	}

	if( prog & GREY )
	{
		draw_box( adm, coo, ro, wid, ro+((prog & EDIT) ? 8 : 6), GREY );
	}

	if( prog & INV )
	{
		draw_box( adm, coo, ro, wid, ro+((prog & EDIT) ? 8 : 6), INV );
	}

	if( prog & UNDERLINED )
	{
		draw_line( adm, coo, ro+8, co-1, ro+8, OR );
	}

	return( co );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
unsigned char display_number( float data, float min, float max, float con,
		unsigned int dp, unsigned int xc, unsigned int yc, unsigned int width, unsigned int just,
		struct NumericFont const *font, void *factor, int ftype )
{
	static float sdata;
	static unsigned int len;

	unsigned int ldp, ui, ed, ch, x1, x2, wi, hi;
	unsigned char *pt, tstring[16];
	float tempf;

	// get edit box size
	hi = font->height + yc - 1;
	if( (just & dn_JUST) == RIGHT )
	{
		x1 = xc - width;
		x2 = xc;	
	}  
	else
	{
		x1 = xc - 1;
		x2 = xc + width;	
	}

	// clear space for number (including invert box)
	draw_box( adm, x1, yc-1, x2, hi+1, CLEAR );

	// check for fit
	ch = font->gap - 1; // character spacing
	wi = font->bitmap[0][0] + ch; // character width + space
	ed = ((width + ch) / wi); // maximum length of string in characters
	
	ldp = 0;
	data *= con;
	if( dp )
	{
		tempf = fabs( data );
		
		ui = 0;
		// count number of digits before dp
		while( tempf >= 10.0 )
		{
			tempf /= 10.0;
			ui++;
		}
		
		// width needed to display number (3 characters needed for 1 decimal place, 4 for 2 etc)
		// digits before + decimal point + digits after 0.0, 0.00, 0000.00 
		wi = ui + dp + 2;
		if( data < 0.0 )
		{
			wi++;
		}
		
		// needed width > available, reduce dp to fit if possible - else defaults to 0
		if( wi > ed )
		{
			ch = wi - ed;
			if( dp > ch )
			{
				ldp = dp - ch;
			}
			
		}
		else
		{
			ldp = dp;
		}

	}

	if( (ftype & PINC) && (just & EDIT) ) // simple increment
	{
		if( editing )
		{
			if( key & (SW_NEW | SW_REP) )
			{
			key &= ~(SW_NEW | SW_REP); // mask off status bits
				if( key == SW_CR )
				{
					if( sdata < max )
					{
						sdata++;
					}
				}
				else if( key == SW_CL )
				{
					if( sdata > min )
					{	
						sdata--;
					}
				}
				else if( key == SW_IL )
				{
					editing = 0xfe; // cancel editing without save - but still copy number to tstring
				}
				else if( key == SW_IR )
				{
					editing = 0xff; // cancel editing with save - but still copy number to tstring
				}
			}
		}
		else if( (just & EDSCAPE) || (key == (SW_NEW | SW_IR)) )
		{
			sdata = data;
			editing = 1;
			looptime = 0;
		}
	}  

	if( !(ftype & PINC) && (just & EDIT) && editing )  // digit program
	{
		// editing string so no sprintf needed	
	}
	else
	{	
		if( just & NSIGN )
		{
			pt = "%+.*f";
		}
		else
		{
			pt = "%.*f";
		}    
		ui = sprintf( tstring, pt, ldp, ((ftype & PINC) && (just & EDIT) && editing) ? sdata : data );
		
		// check for fit
		/*ch = font->gap - 1; // character spacing
		wi = font->bitmap[0][0] + ch; // character width + space
		ed = ((width + ch) / wi);*/ // maximum length of string in characters
		
		if( ui > ed )
		{
			tstring[ed] = 0x00;
			while( ed )
			{
				ed--;
				tstring[ed] = '#';				
			}
		}
		
		if( (just & UNDERS) && !editing && (data == 0.0) )
		{
			for( ui = 0; tstring[ui]; ui++ )
			{
				if( isdigit( tstring[ui] ) )
				{
					tstring[ui] = '_';
				}
			}
		}		
	}

	if( !(ftype & PINC) && (just & EDIT) ) // digit program
	{
		if( editing )
		{
			if( key & (SW_NEW | SW_REP) )
			{
				key &= ~(SW_NEW | SW_REP); // mask off status bits
				pt = &number[editing - 1];
				if( key == SW_CU )
				{
					if( *pt == '+' )
					{
						*pt = '-';
					}
					else if( *pt == '-' )
					{
						*pt = '+';
					}
					else if( *pt == '.' )
					{
						// increase multiplier
						if( editing < (len - 1) )
						{
							editing++;	
							ch = *(pt + 1);
							*(pt + 1) = *pt;
							*pt = ch;
						}
					}
					else
					{
						if( (*pt < '0') || (*pt > '9') )
						{
							*pt = '0';
						}
						else
						{
							(*pt)++;
							if( *pt > '9' )
							{
								*pt = '0';
							}
						}
					}
				}
				else if( key == SW_CD )
				{
					if( *pt == '+' )
					{
						*pt = '-';
					}
					else if( *pt == '-' )
					{
						*pt = '+';
					}
					else if( *pt == '.' )
					{
						// decrease multiplier
						//if( editing > (len - dp) )  // use to set maximum decimal places 
						if( editing > ((just & NSIGN) ? 2 : 1) ) // to program any decimal
						{
							editing--;	
							ch = *(pt - 1);
							*(pt - 1) = *pt;
							*pt = ch;			  
						}
					}
					else
					{
						if( (*pt < '0') || (*pt > '9') )
						{
							*pt = '9';
						}
						else
						{
							(*pt)--;
							if( *pt < '0' )
							{
								*pt = '9';
							}
						}
					}
				}
				else if( key == SW_CL )
				{
					if( editing > 1 )
					{
						editing--;
					}
				}
				else if( key == SW_IL )
				{
					editing = 0xfe; // cancel editing without save - but still copy number to tstring
				}  
				else if( key == SW_CR )
				{
					if( number[editing] != 0x00 ) // end of string
					{
						editing++;	
					}
				}
				else if( key == SW_IR )
				{
					// check for valid number - loop breaks if '_', so end of string not reached
					if( just & UNDERS )
					{
						for( ui = 0; number[ui]; ui++ )
						{
							if( number[ui] == '_' )
							{
								break;
							}
						}
						
						// don't allow exit and save unless a number has been entered
						if( number[ui] == 0x00 )
						{
							editing = 0xff; // cancel editing with save - but still copy number to tstring
						}
					}
					else
					{
						editing = 0xff; // cancel editing with save - but still copy number to tstring
					}
					
				}
			}
		}
		else if( (just & EDSCAPE) || (key == (SW_NEW | SW_IR)) )
		{
			editing = 1;
			looptime = 0;
			strcpy( number, tstring );
			// insert leading zeros
			wi = strlen( number );
			len = width / (font->bitmap[0][0] + font->gap - 1);
			ui = len;
			if( wi < ui )
			{
				while( wi )
				{
					number[ui--] = number[wi--];
				}
				number[ui--] = number[wi--];
				while( ui )
				{
					number[ui--] = '0';
				}
				number[ui] = '0';
			}	
		}

		if( editing )
		{  
			strcpy( tstring, number );
		}
	}

	if( just & EDIT ) // digit & inc program
	{
		if( editing == 0xfe )
		{
			editing = 0;
			if( just & EDSCAPE )
			{
				escape = 2;
			}
		}	
		else if( editing == 0xff )
		{
			editing = 0;
			if( just & EDSCAPE )
			{
				escape = 3;
			}

			if( factor != NULL )
			{
				switch( ftype & PTYPE )
				{
				case PCHAR:
					*(unsigned char *)factor = (ftype & PINC) ? (unsigned char)sdata : (unsigned char)atoi( number );
					if( *(unsigned char *)factor > (unsigned char)max )
					{
						*(unsigned char *)factor = (unsigned char)max;
					}   
					else if( (unsigned char)min && (*(unsigned char *)factor < (unsigned char)min) )
					{
						*(unsigned char *)factor = (unsigned char)min;
					}   
					break;

				case PSHORT:
					*(unsigned short int *)factor = (ftype & PINC) ? (unsigned short int)sdata : (unsigned short int)atoi( number );     
					if( *(unsigned short int *)factor > (unsigned short int)max )
					{
						*(unsigned short int *)factor = (unsigned short int)max;
					}   
					else if( (unsigned short int)min && (*(unsigned short int *)factor < (unsigned short int)min) )
					{
						*(unsigned short int *)factor = (unsigned short int)min;
					}   
					break;

				case PLONG:
					*(unsigned long int *)factor = (ftype & PINC) ? (unsigned long int)sdata : (unsigned long int)atol( number );     
					if( *(unsigned long int *)factor > (unsigned long int)max )
					{
						*(unsigned long int *)factor = (unsigned long int)max;
					}   
					else if( (unsigned long int)min && (*(unsigned long int *)factor < (unsigned long int)min) )
					{
						*(unsigned long int *)factor = (unsigned long int)min;
					}   
					break;

				case PFLOAT:
					*(float *)factor = ((ftype & PINC) ? sdata : atoff( number )) / con;
					if( *(float *)factor > max )
					{
						*(float *)factor = max;
					}   
					else if( *(float *)factor < min )
					{
						*(float *)factor = min;
					}   
					break;
				}

				// if factor is in pFac address space then save to flash
				if( ((unsigned char *)factor >= (unsigned char *)&pFac) &&
					((unsigned char *)factor <  (unsigned char *)(&pFac + sizeof(struct PreservedFactors))) )
				{
					savecal = 1;
				}
			valuechanged = 1;	
			}

		}     
	}

	wi = 0;
	pt = &tstring[0];
	while( *pt )
	{
		ch = *pt;	
		switch( ch )
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			ch = ch - '0';
			break;

		case '.':
			ch = 10;
			break;

		case '+':
			ch = 11;
			break;

		case '-':
			ch = 12;
			break;

		case '#':
			ch = 13;
			break;

		case '_':
			ch = 14;
			break;

		default: // unknown
			ch = 15;
			break;
		}

		// save value + 1, so '0' is not 0x00  
		*pt = ch + 1;  

		// width of character
		wi += font->bitmap[ch][0];

		pt++;

		// last character needs no space
		if( *pt )
		{
			wi += font->gap - 1;
		}	
	}

	if( (just & dn_JUST) == RIGHT )
	{
		xc -= wi;
	}  

	pt = &tstring[0];
	ed = 1;
	while( *pt )
	{
		ch = *pt - 1;

		write_bitmap( adm, font->bitmap[ch], xc, yc, OR );
		wi = xc;
		xc += font->bitmap[ch][0];

		if( !(ftype & PINC) && (just & EDIT) && editing && (editing == ed) )
		{
			if( !key && !half_second )
			{	
				draw_box( adm, wi-1, yc-1, xc, hi+1, INV ); // invert character	
				//draw_box( adm, x1, yc-1, x2, hi+1, INV ); // invert whole string
				//draw_box( adm, wi-1, yc-1, xc, hi+1, CLEAR ); // blank character
			}
			//draw_box( adm, wi, hi+1, xc-1, hi+2, ch ? EMPTY : CLEAR ); // underscore	
		}

		// check to see if last character, if not add gap
		pt++;
		if( *pt )
		{
			xc += font->gap - 1;
		}
		ed++;  
	}

	if( (ftype & PINC) && (just & EDIT) && editing ) // simple increment & editing, flash all
	{
		if( !half_second )
		{	
			draw_box( adm, x1, yc-1, x2, hi+1, INV ); // flash all
		}
	}

	//if( (just & EDIT) && !editing )
	//  {	
	//  draw_box( adm, x1, yc-1, x2, hi+1, INV );
	//  }  

	return( xc );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void draw_frame( unsigned char *pt )
{
	unsigned int i;	

	// clear global display buffer
	for( i = 0; i < GRAPHICS_SIZE; i++ )
	{
		*(pt+i) = 0x00;
	}
	// draw empty frame  
	draw_line( pt, 0,   0, 159,   0, OR );
	draw_line( pt, 0,   0,   0, 127, OR );
	draw_line( pt, 0, 127, 159, 127, OR );
	draw_line( pt, 159, 0, 159, 127, OR );
}    
/*-----------------------------------------------------------*/


#ifndef WINDOWS
/* --- Hardware specific functions below --- */
/*-----------------------------------------------------------*/
void WriteDisplay( unsigned char *pts )
{
	unsigned char *lptd;
	unsigned char command[4], map[160];
	register unsigned char mask;
	register unsigned int c, j, page, pageoffset;

	// set up command Column 0, Page 0 
	command[0] = 0x00; // set Column LSN (0x00 | '0000CCCC')
	command[1] = 0x10; // set Column MSN (0x10 | '0000CCCC') 240 columns max
	command[2] = 0x60; // set Page LSN (0x60 | '0000PPPP')
	// start on page 16 for PM5718 240x128
	command[3] = 0x71; // set Page MSN (0x70 | '00000PPP') 80 pages max
	// send command
	SPI_SendString( CS_DISCOM, command, 4 );

	// write to 64 pages (128 lines)
	for( page = 0; page < 64; page++ )
	{
		// set up command - set column 40
		command[0] = 0x08; // set Column LSN (0x00 | '0000CCCC')
		command[1] = 0x12; // set Column MSN (0x10 | '0000CCCC') 240 columns max
		// send command
		SPI_SendString( CS_DISCOM, command, 2 );

		// remap for Ultra UC1611
		lptd = &map[0];
		pageoffset = page * ((160 / 8) * 2); // page * bytes per line * lines per page
		for( c = 1; c <= 160; c++ ) // columns
		{
			mask = (unsigned char)(0x01 << ((c - 1) % 8));

			// first pixel on first line
			j = pageoffset + ((160 - c) / 8); // result
			if( pts[j] & mask )
			{
#if( SOFTWARE == 100 )    	
				/* Use programmable defaults for test instrument */
				*lptd = (pRun.PixelMask & 0x0f);
#else    	
				/* Use fixed defaults for Production Instruments */
				*lptd = (DISPLAY_DEFAULT_MASK & 0x0f);
#endif
			}
			else
			{
				*lptd = 0x00;
			}  

			// next pixel from line below 
			// j = pageoffset + ((320 - c) / 8); // result
			j += (160 / 8); // + bytes per line
			if( pts[j] & mask )
			{
#if( SOFTWARE == 100 )    	
				/* Use programmable defaults for test instrument */
				*lptd |= (pRun.PixelMask & 0xf0);
#else    	
				/* Use fixed defaults for Production Instruments */
				*lptd |= (DISPLAY_DEFAULT_MASK & 0xf0);
#endif
			}
			lptd++;
		}
		// send data  
		SPI_SendString( CS_DISDAT, map, 160 );
	}

}
/*-----------------------------------------------------------*/

// SPI0 Setup
/*-----------------------------------------------------------*/
void SPI0Setup( void )
{
	// clocks & pins setup in SystemInit_GPIO()

	Set_SPI_CS( CS_DISDAT, CS_HIGH );
	Set_SPI_CS( CS_DISCOM, CS_HIGH );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void ReInitialiseDisplay( unsigned char mode )
{
	unsigned char *pt, command[40];

	pt = &command[0];

	// initialise display
#if( SOFTWARE == 100 )
	/* Use programmable defaults for test instrument */
	*pt++ = 0x2c | DISPLAY_DEFAULT_CHARGE_PUMP; // set Internal Charge Pump
	*pt++ = 0x28 | pRun.Power; // set Panel Loading
	*pt++ = 0xe8 | pRun.Bias; // set Bias Ratio
	*pt++ = 0x24 | pRun.Tempcomp; // set Temperature Compensation
	*pt++ = 0x81; // set Gain/Pot (2 bytes 0x81 + 'GGPPPPPP')
	*pt++ = pRun.Gain | pRun.Contrast;
	*pt++ = 0x84 | DISPLAY_DEFAULT_PARTIAL; // sets Mux Rate depending on DST/DEN
	*pt++ = 0xd0 | pRun.GrayScale; // set gray scale mode
	*pt++ = 0xa0 | pRun.LineRate;  // set linerate
#else
	/* Use fixed defaults for Production Instruments */
	*pt++ = 0x2c | DISPLAY_DEFAULT_CHARGE_PUMP; // set Internal Charge Pump
	*pt++ = 0x28 | DISPLAY_DEFAULT_PANEL_LOADING; // set Panel Loading
	*pt++ = 0xe8 | DISPLAY_DEFAULT_BIAS_RATIO; // set Bias Ratio
	*pt++ = 0x24 | DISPLAY_DEFAULT_TEMPCOMP; // set Temperature Compensation
	*pt++ = 0x81; // set Gain/Pot (2 bytes 0x81 + 'GGPPPPPP')
	*pt++ = DISPLAY_DEFAULT_GAIN | pRun.pr.Contrast[pRun.pr.DayNight];
	*pt++ = 0x84 | DISPLAY_DEFAULT_PARTIAL; // sets Mux Rate depending on DST/DEN
	*pt++ = 0xd0 | DISPLAY_DEFAULT_GRAY_SCALE; // set gray scale mode
	*pt++ = 0xa0 | DISPLAY_DEFAULT_LINE_RATE;  // set linerate
#endif


	*pt++ = 0xf2;  // set DST (2 bytes 0xf2 + 0-159)
	*pt++ = DISPLAY_DEFAULT_STARTLINE;

	*pt++ = 0xf3; // set DEN (2 bytes 0xf3 + 0-159)
	*pt++ = DISPLAY_DEFAULT_ENDLINE;

	*pt++ = 0xf1;  // set CEN (2 bytes 0xf1 + 0-159)
	*pt++ = DISPLAY_DEFAULT_COMEND;

	*pt++ = 0xc0 | DISPLAY_DEFAULT_MAPPING;  // set LCD mapping MY/MX/MSF
	*pt++ = 0x88 | DISPLAY_DEFAULT_RAMCONTROL;  // set RAM address control

	*pt++ = 0x32; // set MAX column address (2 bytes 0x32 + 0-239)
	*pt++ = DISPLAY_DEFAULT_MAXCOLUMN;

	// extra commands
	*pt++ = 0x40;  // set scroll lines LSB - lower 4 bits
	*pt++ = 0x50;  // set scroll lines MSB - upper 4 bits
	*pt++ = 0xa4;  // set all pixels on = 0
	*pt++ = 0xa6;  // set inverse display = 0

	*pt++ = 0xee;  // reset cursor update mode

	/*
	*pt++ = 0x30;  // set Advanced Product Configuration 0
	*pt++ = 0x00;
	*pt++ = 0x31;  // set Advanced Product Configuration 1
	*pt++ = 0x00;

	*pt++ = 0xe4;  // set Test Control 0
	*pt++ = 0x00;
	*pt++ = 0xe5;  // set Test Control 1
	*pt++ = 0x00;
	*pt++ = 0xe6;  // set Test Control 2
	*pt++ = 0x00;
	*pt++ = 0xe7;  // set Test Control 3
	*pt++ = 0x00;
	*/

	if( mode == DISPLAY_ON )
	{
		*pt++ = 0xa8 | DISPLAY_DEFAULT_COLUMNS; // turn on seg drivers (n x 80columns)
	}

	// send display setup commands
	SPI_SendString( CS_DISCOM, command, (unsigned int)(pt - &command[0]) );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void InitialiseDisplay( void )
{
	unsigned int ix;
	unsigned char command[4];

	// reset display
	command[0] = 0xe2;
	SPI_SendString( CS_DISCOM, command, 1 );

	// wait at least 20ms after power on before initilizing display - Reset Could be running
	vTaskDelay( 20 / portTICK_RATE_MS ); // wait 20ms

	ReInitialiseDisplay( DISPLAY_OFF );

	// clear global display buffer
	for( ix = 0; ix < GRAPHICS_SIZE; ix++ )
	{
		adm[ix] = 0x00;
	}

	// clear display driver memory - before turning display on
	WriteDisplay( adm );

	// turn display on after clearing display memory
	command[0] = 0xa8 | DISPLAY_DEFAULT_COLUMNS; // turn on seg drivers (n x 80columns)
	SPI_SendString( CS_DISCOM, command, 1 );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
static void vDisplayTask( void *pvParameters )
{
	unsigned int lastkey;
	unsigned char count, reinit;

	//long int li;
	//unsigned char tstring[16];

	portTickType xLastWakeTime;

	void (*old_window)( unsigned int key );

	// wait at least 15ms after power on before initilizing display - internal Reset could be running
	vTaskDelay( 15 / portTICK_RATE_MS ); // wait 15ms

	// setup SPI0 for display
	SPI0Setup();

	// setup display
	InitialiseDisplay();

	/* if Brightness is Zero PIO mode is used, else peripheral PWM is used, Backlight run at 500Hz */
	SetOutputPWM( PWM_BACK, 500, (unsigned short)pRun.pr.Brightness[pRun.pr.DayNight] * 100 );
	ResetSequenceFactors( SEQ_ALL );
	// key illumination 100%
	SetOutputPWM( PWM_ILLU, 500, PWM_DUTY100 );

	// set current time
	xLastWakeTime = xTaskGetTickCount();
	
	while( 1 )
	{
		vTaskDelayUntil( &xLastWakeTime, (100 / portTICK_RATE_MS) );

		// bit high if not pressed
		key = ~GPIOD->IDR & SW_MASK;

		/* --- Hardware specific functions above --- */
#else
		/* --- Windows specific functions --- */
		void sim_startup( void )
		{
			
			SetStartupDefaults();
			
			PowerSwitchedOff = 0;
			
			looptime = 0;
		}

		extern void WriteToScreen( void );

		void sim_main( unsigned int keypress )
		{
			static unsigned int lastkey;
			static unsigned char count;
			unsigned int last_alarms;

			//unsigned char tstring[16];
			
			void (*old_window)( unsigned int keypress );

			if( keypress != (int)-1 )
			{
				key = keypress;
			}
			else
			{
				key = lastkey & ~(SW_NEW | SW_REP);
			}

#endif

#ifndef WINDOWS
        	// Calculate Analogue Input Rolling Averages:
        	// Bale Diameter (AnIP1); (Bale) Density Pressure (AnIP2); Bale Shape Indicator (AnIP3); Knife Pressure (AnIP4)
        	CalculateAnalogInputRollingAverages();

        	SupplyVolts =  (float)AtoDmux[4].smooth * ATOD_VIN_SCALE;
#endif


			if( key != (lastkey & ~(SW_NEW | SW_REP)) )
			{
				key |= SW_NEW;
				if( key & ~(SW_NEW | SW_REP))
				{
					buzz_length = KEY_BEEP; // 48 mS beep if new key pressed
					looptime = 0;
				}
				else /* no key */
				{
					swrel = 0;
				}
				count = 5;     // 5 counts for first key
			}
			else if( count ) 
			{
				count--;	
			}
			else if( key ) 
			{
				key |= SW_REP;
				count = 1;
			}    
			lastkey = key;  

			old_window = main_window;

			half_second = (looptime / 5) & 0x01;
			
			main_window( key );

			looptime++;
			if( looptime > 249 )
			{
				looptime = 0;
			}

			swrel++; //incremented every 100 ms
			if( swrel > 239 )
			{
				swrel = 0;
			}


			if(  // Dont turn off in Main/Home Screen 2 ((main_window != mainscreen)) &&
						(main_window != screen100) &&                      // Dont turn off in Machine Menu
						(main_window != screen110) &&                      // Dont turn off in Machine Menu - Bale Setup Menu
						(main_window != screen111) &&                      // Dont turn off in Machine Menu - Machine Setup Menu
						(main_window != screen112) &&                      // Dont turn off in Machine Menu - Bale Count Menu
						(main_window!= screen113) &&                      // Dont turn off in Machine Menu - Operator Setup Menu
						(main_window != screen120) &&                      // Dont turn off in Lube Count Reset Screen
						(main_window != screen121) &&                      // Dont turn off in Bale Count Reset Screen
	#if 0 // Removed -- Dia pot zero value fixed
						(window != screen123) &&                      // Dont turn off in Dia Pot Zero Reset Screen
	#endif
						(main_window != screen124) &&                      // Dont turn off in Fill Pot Zero Reset Screen (AKA Bale Shape Indicator Zero)
						(main_window != screen125) &&                      // Dont turn off in Net Metres Reset Screen
						(main_window != screen131) &&                      // Dont turn off in Operator Setup Menu - Brightness & Contrast Adjust Menu
						(main_window != screen132) &&                      // Dont turn off in Operator Setup Menu - Time & Date Adjust Menu
						(main_window != screen140) &&                      // Dont turn off in Technician Menu - PIN entry
						(main_window != screen141) &&                      // Dont turn off in Technician Menu
						(main_window != screen142) &&                      // Dont turn off in Technician Menu - Baler Full
						(main_window != TwineSetupScreen) &&               // Dont turn off in Technician Menu - Twine Setup
						(main_window != screen143) &&                      // Dont turn off in Technician Menu - Netting Setup
						(main_window != screen144) &&                      // Dont turn off in Technician Menu - Diameter Setup
						(main_window != screen145) &&                      // Dont turn off in Technician Menu - Density Setup
						(main_window!= screen146) &&                      // Dont turn off in Technician Menu - Knives Setup
						(main_window != screen147) &&                      // Dont turn off in Technician Menu - Reset Defaults
						(main_window != screen148) &&                      // Dont turn off in Technician Menu - Formation 2
						((main_window !=mainscreen) || (SecondScreen != 1)) &&
						(main_window != screen151) )                       // Dont turn off in Diagnostics - Outputs
				{
					if( key & SW_OL )
					{
						offtime++;

						if( offtime == 8 )
						{
							main_window = screen99;
						}
					}
					else
					{
						offtime = 0;
					}
				}

				if( !key )
				{
					if( reverttime < 250 )
					{
						reverttime++;
					}

					if( reverttime >= 200 )
					{
						if( (main_window != mainscreen) &&  // Not Main/Home Screen
	#if 0 // Restoring automatic return to Main screen after period of inactivity
								(window != screen100) &&   // Not in Machine Menu
								(window != screen110) &&   // Not in Bale Setup Menu
								(window != screen111) &&   // Not in Machine Setup Menu
								(window != screen112) &&   // Not in Bale Count Menu
								(window != screen113) &&   // Not in Operator Setup Menu
	#endif
								(main_window != screen120)  &&  // Not in Lube Count Reset Screen
								(main_window != screen121)  &&  // Not in Bale Count Reset Screen
								(main_window != screen124)  &&  // Not in Fill Pot Zero Reset Screen (AKA Bale Shape Indicator Zero)
								(main_window != screen125)  &&  // Not in Net Metres Reset Screen
	#if 0 // Restoring automatic return to Main screen after period of inactivity
								(window != screen131)  &&  // Not in Brightness & Contrast Adjust Menu
								(window != screen132)  &&  // Not in Time & Date Adjust Menu
								(window != screen140)  &&  // Not Technician Menu - PIN entry
	#endif
								(main_window != screen141)  &&  // Not Technician Menu
								(main_window != screen142)  &&  // Not Technician Menu - Baler Full
								(main_window != screen143)  &&  // Not Technician Menu - Netting Setup
								(main_window != TwineSetupScreen) &&               // Dont turn off in Technician Menu - Twine Setup
								(main_window != screen144)  &&  // Not Technician Menu - Diameter Setup
								(main_window != screen145)  &&  // Not Technician Menu - Density Setup
								(main_window != screen146)  &&  // Not Technician Menu - Knives Setup
								(main_window != screen147)  &&  // Not Technician Menu - Reset Defaults
								(main_window != screen148)  &&  // Not Technician Menu - Formation 2
								(main_window != screen150)  &&  // Not Diagnostics - Digital Inputs
								(main_window != screen152)  &&  // Not Diagnostics - Analogue Inputs
								(main_window != CANDiagnostics)  &&  // Not Diagnostics - CAN Diagnostics
								(main_window != screen151)    ) // Not Diagnostics - Outputs
						{
							main_window = mainscreen; // Main/Home Screen
							nextwindow = 0;
							editing = 0;
							leaf = 0;
							cont_beep = 1;
							buzz_length = BACK_TO_MAIN_SCR_BEEP;
						}

					}
					else if( reverttime >= 20 )
					{
						if( main_window == screen99 )
						{
							main_window = mainscreen; // Main/Home Screen
							nextwindow = 0;
							editing = 0;
							leaf = 0;
						}
					}
				}
				else if( key )
				{
					reverttime = 0;
				}


				if( deftime )
				{
					deftime--;
					if( deftime == 0 )
					{
						main_window = mainscreen; // Main/Home Screen
					}
				}

				if( (main_window != screen00) && (main_window != screen150) && (main_window != screen151) && (main_window != screen152) && (main_window != CANDiagnostics))
				{
					Inputs();

	#ifndef WINDOWS
					CheckAlarms();
	#endif

					Sequence();

					//if((pFac.pf.GW302_TwineOption == TO_ON) && (pFac.pf.GW302_NetTwineOption == TO_TWINE))
					//	TwineSequence();

				}


			if( escape )
			{
				escape = 0;
				if( windowdepth )
				{
					windowdepth--;
					main_window = PreviousWindow[windowdepth].window;
					selection = PreviousWindow[windowdepth].selection;
				}
				else
				{
					main_window = default_window;
					selection = 0;
				}
			}
			// window has changed
			else if( main_window != old_window )
			{	
				// save previous window if leaf clear & nextwindow is 0 or NEXTWINDOW_ESCAPE
				if( !leaf && (nextwindow != NEXTWINDOW_NOESCAPE) )
				{
					PreviousWindow[windowdepth].window = old_window;
					PreviousWindow[windowdepth].selection = selection;  
					if( windowdepth < (MAX_WINDOWDEPTH - 1) )
					{
						windowdepth++;
					}
				}
				selection = 0;
			}
			
			if( main_window != old_window )
			{
				startup = 0;
				nextwindow = 0;	  	 
				editing = 0;
				okset = 0;
				leaf = 0;

				key &= ~(SW_NEW | SW_REP);
				if( main_window )
				{
					pin_window = NULL;
					main_window( 0 ); // no key action
				}
			}

#ifndef WINDOWS  
			//sprintf( tstring, "<%04X-%04X>", alarms_on, alarms_masked );
			//display_f1_text( tstring, TS15, 40, 3, FIXED );

			// re initilise display setup registers every second
			if( reinit != 0 )
			{
				reinit--;
			}
			else
			{
				reinit = 10;
				ReInitialiseDisplay( DISPLAY_ON );
			}
			
			WriteDisplay( adm );
			
			if( pRun.pr.PipsOn )
			{
				PipsGen();
			}
		} // end of while( 1 )
#else
		//sprintf( tstring, "<%04X-%04X>", alarms_on, alarms_masked );
		//display_f1_text( tstring, TS15, 40, 4, FIXED );
		
		WriteToScreen();
#endif

	}
/*-----------------------------------------------------------*/

#ifndef WINDOWS
/*-----------------------------------------------------------*/
void vStartDisplayTask( unsigned portBASE_TYPE uxPriority )
{

	/* Spawn the task. */
	xTaskCreate( vDisplayTask, "DISPLAY", configDISPLAY_STACK_SIZE, (void *)NULL, uxPriority, (xTaskHandle *)NULL );
}
/*-----------------------------------------------------------*/
#endif

void clip_box( unsigned char xp1, unsigned char yp1, unsigned char xp2, unsigned char yp2 )
{
draw_line( adm, xp1, yp1, xp1, yp1, CLEAR );
draw_line( adm, xp2, yp1, xp2, yp1, CLEAR );
draw_line( adm, xp1, yp2, xp1, yp2, CLEAR );
draw_line( adm, xp2, yp2, xp2, yp2, CLEAR );
}
