/* bpe.c - rewritten to handle parameterised command line input */

/* from compress.c */
/* Copyright Philip Gage */
/* printed in 'The C Users Journal' February, 1994 */

#include <stdio.h>
#include <stdlib.h>

#define BLOCKSIZE 10000 	/* maximum block size */
#define HASHSIZE 8192 		/* size of hash table */
#define MAXCHARS 220 		/* char set per block */
#define THRESHOLD 3 		/* minimum pair count */

unsigned char buffer[BLOCKSIZE];		/* data block */
unsigned char leftcode[256];		/* pair table */
unsigned char rightcode[256];		/* pair table */
unsigned char left[HASHSIZE];		/* hash table */
unsigned char right[HASHSIZE];		/* hash table */
unsigned char count[HASHSIZE];		/* pair count */
int size;		/* size of current data block */

/* function prototypes */
int lookup (unsigned char, unsigned char, int );
int fileread (FILE *, int, int, int);
void filewrite (FILE *);
void compress (FILE *, FILE *, int, int, int, int);

/* return index of character pair in hash table */
/* deleted nodes have a count of 1 for hashing */
int lookup (unsigned char a, unsigned char b, int hs)
{
  int index;	/* ?  - will add question marks until I understand each variable */


  /* compute hash key from both characters */
  index = (a ^ (b << 5)) & (hs-1);	/* ? */
/* if b = 10110101 then '(b << 5)' --> b = 10100000. */
/* ie shift the bits in b left by five positions and fill holes with zeros */

  /* search for pair or first empty slot */
  while ((left[index] != a || right[index] != b) && count[index] != 0)
  {
    index = (index + 1) & (hs - 1);
  }

  left[index] = a;
  right[index] = b;
  return index;
}

/* read next block from input file into buffer */
int fileread (FILE *input, int bs, int hs, int mc)
{
  int c, index, used=0;

  /* reset hash table and pair table */
  for (c = 0; c < hs; c++)
    count[c] = 0;
  for (c = 0; c < 256; c++)
  {
    leftcode[c] = c;
    rightcode[c] = 0;
  }
  size = 0;

  /* read data until full or few unused chars */
  while (size < bs && used < mc && (c = getc(input)) != EOF)
  {
    if (size > 0)
    {
      index = lookup(buffer[size-1], c, hs);
      if (count[index] < 255) 
      {
        ++count[index];
      }
    }
    buffer[size++] = c;

    /* use right code to flag data chars found */
    if (!rightcode[c])
    {
      rightcode[c] = 1;
      used++;
    }
  }
  return c == EOF;
}

/* write each pair table and data block to output */
void filewrite( FILE *output )
{
  int i, len, c = 0;

  /* for each character 0..255 */
  while ( c < 256 ) 
  {
    /* if not a pair code, count run of literals */
    if ( c == leftcode[c] )
    {
      len = 1; c++;
      while ( len < 127 && c < 256 && c == leftcode[c])
      {
        len++; c++;
      }
      putc( len + 127, output ); 
      len = 0;
      if ( c == 256 ) break;
    }
   
    /* else count run of pair codes */
    else
    {
      len = 0;
      c++;


/* original, will add extra brackets per compiler suggestions:      while ( len < 127 && c < 256 && c != leftcode[c] || len < 125 && c < 254 && c+1 != leftcode[c+1]) */
      while (( len < 127 && c < 256 && c != leftcode[c]) || (len < 125
&& c < 254 && c+1 != leftcode[c+1]))
      {
        len++;
        c++;
      }
      putc(len, output);
      c -= len+1;
    }

    /* write range of pairs to output */
    for ( i = 0; i <= len; i++ )
    {
      putc(leftcode[c], output);
      if ( c != leftcode[c] )
      { putc(rightcode[c], output); }
      c++;
    }
  }
  /* write size bytes and compressed data block */
  putc(size/256, output);
  putc(size%256, output);
  fwrite(buffer, size, 1, output);
}

/* compress from input file to output file */
void compress( FILE *infile, FILE *outfile, 
               int bs, int hs, int mc, int th )
{
  int leftch, rightch, code, oldsize;
  int index, r, w, best, done = 0;

  /* compress each data block until end of file */
  while ( !done )
  {
    done = fileread(infile, bs, hs, mc);
    code = 256;
   
    /* compress this block */
    for(;;)
    {
      /* get next unused chr for pair code */
      for ( code--; code >= 0; code-- )
      {
        if ( code == leftcode[code] && !rightcode[code] )
        {
          break;
        }
      }
 
      /* must quit if no unused chars left */
      if ( code < 0 )
      {
        break;
      }

      /* find most frequent pair of chars */
      for ( best = 2, index = 0; index < hs; index++ )
      {
        if (count[index] > best) 
        {
          best = count[index];
          leftch = left[index];
          rightch = right[index];
        }
      }
    
      /* done if no more compression possible */
      if ( best < th )
      {
        break;
      }
   
      /* Replace pairs in data, adjust pair counts */
      oldsize = size - 1;
      for ( w = 0, r = 0; r < oldsize ; r++ )
      {
        if (buffer[r] == leftch && buffer[r+1] == rightch)
        {
          if ( r > 0 )
          {
            index = lookup(buffer[w-1], leftch, hs);
            if ( count[index] > 1 )
            {
              --count[index];
            }
            index = lookup( buffer[w-1], code, hs );
            if ( count[index] < 255 ) 
            {
              ++count[index];
            }
          }
          if ( r < oldsize - 1 )
          {
            index = lookup( rightch, buffer[r+2] , hs);
            if ( count[index] > 1 )
            {
              --count[index];
            }
            index = lookup( code, buffer[r+2], hs );
            if ( count[index] < 255 ) 
            {
              ++count[index];
            }
          }
          buffer[w++] = code;
          r++;
          size--;
        }
        else
        {
          buffer[w++] = buffer[r];
        }
      }
      buffer[w] = buffer[r];

      /* add to pair substitution table */
      leftcode[code] = leftch;
      rightcode[code] = rightch;
    
      /* delete pair from hash table */
      index = lookup( leftch, rightch, hs );
      count[index] = 1;
    }
    filewrite( outfile );
  }
}

int main (int argc, char *argv[] )
{
  FILE *infile, *outfile;
/* argc = 7              */
/*   argv[0] = command   */
/*   argv[1] = infile    */
/*   argv[2] = outfile   */
/*   argv[3] = BLOCKSIZE */
/*   argv[4] = HASHSIZE  */
/*   argv[5] = MAXCHARS  */
/*   argv[6] = THRESHOLD */
  int bs = 20000; /* maxval */
  int hs = 16384; /* maxval */
  int mc = 200;   /* default value */
  int th = 3;     /* default min */


  if (argc != 7)
  {
    printf("Usage: bpe infile outfile blocksize hashsize maxchars threshold\n");
    printf("typical: bpe infile outfile 5000 4096 200 3\n");
  }
  else
  {
    if (( infile = fopen( argv[1], "rb" )) == NULL )
    {
      printf("Error opening input %s\n",argv[1]);
    }
    else
    {
      if (( outfile = fopen( argv[2], "wb" )) == NULL )
      {
        printf("Error opening output %s\n",argv[2]);
      }
      else
      {
        bs = atoi( argv[3] );
        hs = atoi( argv[4] );
        mc = atoi( argv[5] );
        th = atoi( argv[6] );
        /* because these inputs come from the command line generated
           by clustor, I have included very little error checking here
        */    

        compress( infile, outfile, bs, hs, mc, th );
        fclose( outfile );
        fclose( infile );
      }
    }
  }
  return 0;
}

/* end of file */



   
       
