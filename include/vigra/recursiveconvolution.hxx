/************************************************************************/
/*                                                                      */
/*               Copyright 1998-2000 by Ullrich Koethe                  */
/*       Cognitive Systems Group, University of Hamburg, Germany        */
/*                                                                      */
/*    This file is part of the VIGRA computer vision library.           */
/*    You may use, modify, and distribute this software according       */
/*    to the terms stated in the LICENSE file included in               */
/*    the VIGRA distribution.                                           */
/*                                                                      */
/*    The VIGRA Website is                                              */
/*        http://kogs-www.informatik.uni-hamburg.de/~koethe/vigra/      */
/*    Please direct questions, bug reports, and contributions to        */
/*        koethe@informatik.uni-hamburg.de                              */
/*                                                                      */
/*  THIS SOFTWARE IS PROVIDED AS IS AND WITHOUT ANY EXPRESS OR          */
/*  IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED      */
/*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. */
/*                                                                      */
/************************************************************************/
 
 
#ifndef VIGRA_RECURSIVECONVOLUTION_HXX
#define VIGRA_RECURSIVECONVOLUTION_HXX

#include <cmath>
#include <vector>
#include "vigra/utilities.hxx"
#include "vigra/numerictraits.hxx"
#include "vigra/imageiteratoradapter.hxx"

namespace vigra {

/********************************************************/
/*                                                      */
/*         Recursive convolution functions              */
/*                                                      */
/********************************************************/

/** @heading Recursive convolution functions
    These functions implement the exponential filter
    and its first and second derivatives as a recursive
    operation. This is very fast, and the speed does not depend on the 
    filter size. 

    @memo Recursive implementation of the exponential filter and its derivatives
*/
//@{

/********************************************************/
/*                                                      */
/*                    recursiveSmoothLine               */
/*                                                      */
/********************************************************/

/** Performs a 1 dimensional recursive convolution of the source signal 
    using an exponential  #exp(-abs(x)/scale)# as a kernel.
    
    The signal's value_type (SrcAccessor::value_type) must be a
    linear space over the kernel's value_type (KernelAccessor::value_type),
    i.e. addition of source values, multiplication with kernel values,
    and NumericTraits must be defined. 
    
    {\bf Declaration:}
    
    \begin{verbatim}
    namespace vigra {
        template <class SrcIterator, class SrcAccessor,
              class DestIterator, class DestAccessor>
        void recursiveSmoothLine(SrcIterator is, SrcIterator isend, SrcAccessor as,
                     DestIterator id, DestAccessor ad, double scale)
    }
    \end{verbatim}
    
    {\bf Usage:}
    
    Include-File:
    \URL[vigra/recursiveconvolution.hxx]{../include/vigra/recursiveconvolution.hxx}\\
    Namespace: vigra
    
    
    \begin{verbatim}
    vector<float> src, dest;    
    ...
    
    vigra::DefaultAccessor<vector<float>::iterator, float> FAccessor;
    
    
    vigra::recursiveSmoothLine(src.begin(), src.end(), FAccessor(), 
                        dest.begin(), FAccessor(), 3.0);
    \end{verbatim}

    {\bf Required Interface:}
    
    \begin{verbatim}
    RandomAccessIterator is, isend;
    RandomAccessIterator id;
    
    SrcAccessor src_accessor;
    DestAccessor dest_accessor;
    
    NumericTraits<SrcAccessor::value_type>::RealPromote s = src_accessor(is);
    double d;
    
    s = s + s;
    s = d * s;

    dest_accessor.set(
        NumericTraits<DestAccessor::value_type>::fromRealPromote(s), id);

    \end{verbatim}

    {\bf Preconditions:}
    
    \begin{verbatim}
    scale > 0
    \end{verbatim}

    @memo
*/
template <class SrcIterator, class SrcAccessor,
          class DestIterator, class DestAccessor>
void recursiveSmoothLine(SrcIterator is, SrcIterator isend, SrcAccessor as,
                         DestIterator id, DestAccessor ad, double scale)
{
    int w = isend -is;
    
    int x;
    
    vigra_precondition(scale > 0,
                 "recursiveSmoothLine(): scale must be > 0.\n");

    typedef typename
        NumericTraits<typename SrcAccessor::value_type>::RealPromote 
    TempType;
    typedef NumericTraits<typename DestAccessor::value_type> DestTraits;
    
    std::vector<TempType> vline(w);
    typename std::vector<TempType>::iterator line = vline.begin();
    
#ifndef CMATH_NOT_IN_STD
    double b = std::exp(-1.0/scale);
#else
    double b = exp(-1.0/scale);
#endif
    double norm = (1.0 - b) / (1.0 + b);
    TempType old = (1.0 / (1.0 - b)) * as(is);

    // left side of filter
    for(x=0; x<w; ++x, ++is)
    {
        old = as(is) + b * old;
        line[x] = old;
    }
    
    // right side of the filter
    --is;
    old = (1.0 / (1.0 - b)) * as(is);
    id += w;
    ++is;
    
    for(x=w-1; x>=0; --x)
    {    
        --is;
        --id;
        
        old = b * old;
        ad.set(DestTraits::fromRealPromote(norm * (line[x] + old)), id);

        old = as(is) + old;
    }
}
            
/********************************************************/
/*                                                      */
/*             recursiveFirstDerivativeLine             */
/*                                                      */
/********************************************************/

/** Performs a 1 dimensional recursive convolution of the source signal 
    using the first derivative an exponential  #d/dx exp(-abs(x)/scale)# as 
    a kernel.
    
    The signal's value_type (SrcAccessor::value_type) must be a
    linear space over the kernel's value_type (KernelAccessor::value_type),
    i.e. addition and subtraction of source values, multiplication with 
    kernel values, and NumericTraits must be defined. 
    
    {\bf Declaration:}
    
    \begin{verbatim}
    namespace vigra {
        template <class SrcIterator, class SrcAccessor,
              class DestIterator, class DestAccessor>
        void recursiveFirstDerivativeLine(SrcIterator is, SrcIterator isend, SrcAccessor as,
                     DestIterator id, DestAccessor ad, double scale)
    }
    \end{verbatim}
    
    {\bf Usage:}
    
    Include-File:
    \URL[vigra/recursiveconvolution.hxx]{../include/vigra/recursiveconvolution.hxx}\\
    Namespace: vigra
    
    
    \begin{verbatim}
    vector<float> src, dest;    
    ...
    
    vigra::DefaultAccessor<vector<float>::iterator, float> FAccessor;
    
    
    vigra::recursiveFirstDerivativeLine(src.begin(), src.end(), FAccessor(), 
                        dest.begin(), FAccessor(), 3.0);
    \end{verbatim}

    {\bf Required Interface:}
    
    \begin{verbatim}
    RandomAccessIterator is, isend;
    RandomAccessIterator id;
    
    SrcAccessor src_accessor;
    DestAccessor dest_accessor;
    
    NumericTraits<SrcAccessor::value_type>::RealPromote s = src_accessor(is);
    double d;
    
    s = s + s;
    s = -s;
    s = d * s;

    dest_accessor.set(
        NumericTraits<DestAccessor::value_type>::fromRealPromote(s), id);

    \end{verbatim}

    {\bf Preconditions:}
    
    \begin{verbatim}
    scale > 0
    \end{verbatim}

    @memo
*/
template <class SrcIterator, class SrcAccessor,
          class DestIterator, class DestAccessor>
void recursiveFirstDerivativeLine(SrcIterator is, SrcIterator isend, SrcAccessor as,
                         DestIterator id, DestAccessor ad, double scale)
{
    vigra_precondition(scale > 0,
                 "recursiveFirstDerivativeLine(): scale must be > 0.\n");

    int w = isend -is;
    
    int x;
    
    typedef typename
        NumericTraits<typename SrcAccessor::value_type>::RealPromote 
    TempType;
    typedef NumericTraits<typename DestAccessor::value_type> DestTraits;

    std::vector<TempType> vline(w);
    typename std::vector<TempType>::iterator line = vline.begin();
    
#ifndef CMATH_NOT_IN_STD
    double b = std::exp(-1.0/scale);
#else
    double b = exp(-1.0/scale);
#endif
    double norm = (1.0 - b) * (1.0 - b) / 2.0 / b;
    TempType old = (1.0 / (1.0 - b)) * as(is);

    // left side of filter
    for(x=0; x<w; ++x, ++is)
    {
        old = as(is) + b * old;
        line[x] = -old;
    }
    
    // right side of the filter
    --is;
    old = (1.0 / (1.0 - b)) * as(is);
    id += w;
    ++is;
    
    for(x=w-1; x>=0; --x)
    {    
        --is;
        --id;

        old = as(is) + b * old;

        ad.set(DestTraits::fromRealPromote(norm * (line[x] + old)), id);
    }
}
            
/********************************************************/
/*                                                      */
/*            recursiveSecondDerivativeLine             */
/*                                                      */
/********************************************************/

/** Performs a 1 dimensional recursive convolution of the source signal 
    using the second derivative an exponential  #d2/dx2 exp(-abs(x)/scale)# as 
    a kernel.
    
    The signal's value_type (SrcAccessor::value_type) must be a
    linear space over the kernel's value_type (KernelAccessor::value_type),
    i.e. addition and subtraction of source values, multiplication with 
    kernel values, and NumericTraits must be defined. 
    
    {\bf Declaration:}
    
    \begin{verbatim}
    namespace vigra {
        template <class SrcIterator, class SrcAccessor,
              class DestIterator, class DestAccessor>
        void recursiveSecondDerivativeLine(SrcIterator is, SrcIterator isend, SrcAccessor as,
                     DestIterator id, DestAccessor ad, double scale)
    }
    \end{verbatim}
    
    {\bf Usage:}
    
    Include-File:
    \URL[vigra/recursiveconvolution.hxx]{../include/vigra/recursiveconvolution.hxx}\\
    Namespace: vigra
    
    
    \begin{verbatim}
    vector<float> src, dest;    
    ...
    
    vigra::DefaultAccessor<vector<float>::iterator, float> FAccessor;
    
    
    vigra::recursiveSecondDerivativeLine(src.begin(), src.end(), FAccessor(), 
                        dest.begin(), FAccessor(), 3.0);
    \end{verbatim}

    {\bf Required Interface:}
    
    \begin{verbatim}
    RandomAccessIterator is, isend;
    RandomAccessIterator id;
    
    SrcAccessor src_accessor;
    DestAccessor dest_accessor;
    
    NumericTraits<SrcAccessor::value_type>::RealPromote s = src_accessor(is);
    double d;
    
    s = s + s;
    s = s - s;
    s = d * s;

    dest_accessor.set(
        NumericTraits<DestAccessor::value_type>::fromRealPromote(s), id);

    \end{verbatim}

    {\bf Preconditions:}
    
    \begin{verbatim}
    scale > 0
    \end{verbatim}

    @memo
*/
template <class SrcIterator, class SrcAccessor,
          class DestIterator, class DestAccessor>
void recursiveSecondDerivativeLine(SrcIterator is, SrcIterator isend, SrcAccessor as,
                         DestIterator id, DestAccessor ad, double scale)
{
    vigra_precondition(scale > 0,
                 "recursiveSecondDerivativeLine(): scale must be > 0.\n");

    int w = isend -is;
    
    int x;
    
    typedef typename
        NumericTraits<typename SrcAccessor::value_type>::RealPromote 
    TempType;
    typedef NumericTraits<typename DestAccessor::value_type> DestTraits;
    
    std::vector<TempType> vline(w);
    typename std::vector<TempType>::iterator line = vline.begin();
        
#ifndef CMATH_NOT_IN_STD
    double b = std::exp(-1.0/scale);
#else
    double b = exp(-1.0/scale);
#endif
    double a = -2.0 / (1.0 - b);
    double norm = (1.0 - b) * (1.0 - b) * (1.0 - b) / (1.0 + b);
    TempType old = (1.0 / (1.0 - b)) * as(is);

    // left side of filter
    for(x=0; x<w; ++x, ++is)
    {
        line[x] = old;
        old = as(is) + b * old;
    }
    
    // right side of the filter
    --is;
    old = (1.0 / (1.0 - b)) * as(is);
    id += w;
    ++is;
    
    for(x=w-1; x>=0; --x)
    {    
        --is;
        --id;

        ad.set(DestTraits::fromRealPromote(norm * (line[x] + old + a * as(is))), id);

        old = as(is) + b * old;
    }
}
            
/********************************************************/
/*                                                      */
/*                    recursiveSmoothX                  */
/*                                                      */
/********************************************************/

/** Performs 1 dimensional recursive smoothing in x direction by 
    calling \Ref{recursiveSmoothLine}() for every row of the
    image. See \Ref{recursiveSmoothLine}() for more information about 
    required interfaces and vigra_preconditions.
    
    {\bf Declarations:}
    
    pass arguments explicitly:
    \begin{verbatim}
    namespace vigra {
        template <class SrcImageIterator, class SrcAccessor,
              class DestImageIterator, class DestAccessor>
        void recursiveSmoothX(SrcImageIterator supperleft, 
                  SrcImageIterator slowerright, SrcAccessor as,
                  DestImageIterator dupperleft, DestAccessor ad, 
                  double scale)
    }
    \end{verbatim}
    
    
    use argument objects in conjuction with \Ref{Argument Object Factories}:
    \begin{verbatim}
    namespace vigra {
        template <class SrcImageIterator, class SrcAccessor,
              class DestImageIterator, class DestAccessor>
        void recursiveSmoothX(
            triple<SrcImageIterator, SrcImageIterator, SrcAccessor> src,
            pair<DestImageIterator, DestAccessor> dest, 
            double scale)
    }
    \end{verbatim}
    
    {\bf Usage:}
    
    Include-File:
    \URL[vigra/recursiveconvolution.hxx]{../include/vigra/recursiveconvolution.hxx}\\
    Namespace: vigra
    
    \begin{verbatim}
    vigra::FImage src(w,h), dest(w,h);    
    ...
    
    vigra::recursiveSmoothX(srcImageRange(src), destImage(dest), 3.0);
    
    \end{verbatim}

    @memo
*/
template <class SrcImageIterator, class SrcAccessor,
          class DestImageIterator, class DestAccessor>
void recursiveSmoothX(SrcImageIterator supperleft, 
                      SrcImageIterator slowerright, SrcAccessor as,
                      DestImageIterator dupperleft, DestAccessor ad, 
              double scale)
{
    int w = slowerright.x - supperleft.x;
    int h = slowerright.y - supperleft.y;
    
    int y;
    
    for(y=0; y<h; ++y, ++supperleft.y, ++dupperleft.y)
    {
        RowIterator<SrcImageIterator> rs(supperleft);
        RowIterator<DestImageIterator> rd(dupperleft);

        recursiveSmoothLine(rs, rs+w, accessorAdapter(rs, as), 
                            rd, accessorAdapter(rd, ad), 
                    scale);
    }
}
            
template <class SrcImageIterator, class SrcAccessor,
          class DestImageIterator, class DestAccessor>
inline void recursiveSmoothX(
            triple<SrcImageIterator, SrcImageIterator, SrcAccessor> src,
            pair<DestImageIterator, DestAccessor> dest, 
        double scale)
{
    recursiveSmoothX(src.first, src.second, src.third,
                          dest. first, dest.second, scale);
}
            
/********************************************************/
/*                                                      */
/*                     recursiveSmoothY                 */
/*                                                      */
/********************************************************/

/** Performs 1 dimensional recursive smoothing in y direction by 
    calling \Ref{recursiveSmoothLine}() for every column of the
    image. See \Ref{recursiveSmoothLine}() for more information about 
    required interfaces and vigra_preconditions.
    
    {\bf Declarations:}
    
    pass arguments explicitly:
    \begin{verbatim}
    namespace vigra {
        template <class SrcImageIterator, class SrcAccessor,
              class DestImageIterator, class DestAccessor>
        void recursiveSmoothY(SrcImageIterator supperleft, 
                  SrcImageIterator slowerright, SrcAccessor as,
                  DestImageIterator dupperleft, DestAccessor ad, 
                  double scale)
    }
    \end{verbatim}
    
    
    use argument objects in conjuction with \Ref{Argument Object Factories}:
    \begin{verbatim}
    namespace vigra {
        template <class SrcImageIterator, class SrcAccessor,
              class DestImageIterator, class DestAccessor>
        void recursiveSmoothY(
            triple<SrcImageIterator, SrcImageIterator, SrcAccessor> src,
            pair<DestImageIterator, DestAccessor> dest, 
            double scale)
    }
    \end{verbatim}
    
    {\bf Usage:}
    
    Include-File:
    \URL[vigra/recursiveconvolution.hxx]{../include/vigra/recursiveconvolution.hxx}\\
    Namespace: vigra
    
    \begin{verbatim}
    vigra::FImage src(w,h), dest(w,h);    
    ...
    
    vigra::recursiveSmoothY(srcImageRange(src), destImage(dest), 3.0);
    
    \end{verbatim}

    @memo
*/
template <class SrcImageIterator, class SrcAccessor,
          class DestImageIterator, class DestAccessor>
void recursiveSmoothY(SrcImageIterator supperleft, 
                      SrcImageIterator slowerright, SrcAccessor as,
                      DestImageIterator dupperleft, DestAccessor ad, 
              double scale)
{
    int w = slowerright.x - supperleft.x;
    int h = slowerright.y - supperleft.y;
    
    int x;
    
    for(x=0; x<w; ++x, ++supperleft.x, ++dupperleft.x)
    {
        ColumnIterator<SrcImageIterator> cs(supperleft);
        ColumnIterator<DestImageIterator> cd(dupperleft);

        recursiveSmoothLine(cs, cs+h, accessorAdapter(cs, as), 
                            cd, accessorAdapter(cd, ad), 
                    scale);
    }
}
            
template <class SrcImageIterator, class SrcAccessor,
          class DestImageIterator, class DestAccessor>
inline void recursiveSmoothY(
            triple<SrcImageIterator, SrcImageIterator, SrcAccessor> src,
            pair<DestImageIterator, DestAccessor> dest, 
        double scale)
{
    recursiveSmoothY(src.first, src.second, src.third,
                          dest. first, dest.second, scale);
}
            
/********************************************************/
/*                                                      */
/*              recursiveFirstDerivativeX               */
/*                                                      */
/********************************************************/

/** Recursively calculates the 1 dimensional first derivative in x 
    direction by calling \Ref{recursiveFirstDerivativeLine}() for every 
    row of the image. See \Ref{recursiveFirstDerivativeLine}() for more 
    information about required interfaces and vigra_preconditions.
    
    {\bf Declarations:}
    
    pass arguments explicitly:
    \begin{verbatim}
    namespace vigra {
        template <class SrcImageIterator, class SrcAccessor,
              class DestImageIterator, class DestAccessor>
        void recursiveFirstDerivativeX(SrcImageIterator supperleft, 
                  SrcImageIterator slowerright, SrcAccessor as,
                  DestImageIterator dupperleft, DestAccessor ad, 
                  double scale)
    }
    \end{verbatim}
    
    
    use argument objects in conjuction with \Ref{Argument Object Factories}:
    \begin{verbatim}
    namespace vigra {
        template <class SrcImageIterator, class SrcAccessor,
              class DestImageIterator, class DestAccessor>
        void recursiveFirstDerivativeX(
            triple<SrcImageIterator, SrcImageIterator, SrcAccessor> src,
            pair<DestImageIterator, DestAccessor> dest, 
            double scale)
    }
    \end{verbatim}
    
    {\bf Usage:}
    
    Include-File:
    \URL[vigra/recursiveconvolution.hxx]{../include/vigra/recursiveconvolution.hxx}\\
    Namespace: vigra
    
    \begin{verbatim}
    vigra::FImage src(w,h), dest(w,h);    
    ...
    
    vigra::recursiveFirstDerivativeX(srcImageRange(src), destImage(dest), 3.0);
    
    \end{verbatim}

    @memo
*/
template <class SrcImageIterator, class SrcAccessor,
          class DestImageIterator, class DestAccessor>
void recursiveFirstDerivativeX(SrcImageIterator supperleft, 
                      SrcImageIterator slowerright, SrcAccessor as,
                      DestImageIterator dupperleft, DestAccessor ad, 
              double scale)
{
    int w = slowerright.x - supperleft.x;
    int h = slowerright.y - supperleft.y;
    
    int y;
    
    for(y=0; y<h; ++y, ++supperleft.y, ++dupperleft.y)
    {
        RowIterator<SrcImageIterator> rs(supperleft);
        RowIterator<DestImageIterator> rd(dupperleft);

        recursiveFirstDerivativeLine(rs, rs+w, accessorAdapter(rs, as), 
                                     rd, accessorAdapter(rd, ad), 
                         scale);
    }
}
            
template <class SrcImageIterator, class SrcAccessor,
          class DestImageIterator, class DestAccessor>
inline void recursiveFirstDerivativeX(
            triple<SrcImageIterator, SrcImageIterator, SrcAccessor> src,
            pair<DestImageIterator, DestAccessor> dest, 
        double scale)
{
    recursiveFirstDerivativeX(src.first, src.second, src.third,
                          dest. first, dest.second, scale);
}
            
/********************************************************/
/*                                                      */
/*              recursiveFirstDerivativeY               */
/*                                                      */
/********************************************************/

/** Recursively calculates the 1 dimensional first derivative in y 
    direction by calling \Ref{recursiveFirstDerivativeLine}() for every 
    column of the image. See \Ref{recursiveFirstDerivativeLine}() for more 
    information about required interfaces and vigra_preconditions.
    
    {\bf Declarations:}
    
    pass arguments explicitly:
    \begin{verbatim}
    namespace vigra {
        template <class SrcImageIterator, class SrcAccessor,
              class DestImageIterator, class DestAccessor>
        void recursiveFirstDerivativeY(SrcImageIterator supperleft, 
                  SrcImageIterator slowerright, SrcAccessor as,
                  DestImageIterator dupperleft, DestAccessor ad, 
                  double scale)
    }
    \end{verbatim}
    
    
    use argument objects in conjuction with \Ref{Argument Object Factories}:
    \begin{verbatim}
    namespace vigra {
        template <class SrcImageIterator, class SrcAccessor,
              class DestImageIterator, class DestAccessor>
        void recursiveFirstDerivativeY(
            triple<SrcImageIterator, SrcImageIterator, SrcAccessor> src,
            pair<DestImageIterator, DestAccessor> dest, 
            double scale)
    }
    \end{verbatim}
    
    {\bf Usage:}
    
    Include-File:
    \URL[vigra/recursiveconvolution.hxx]{../include/vigra/recursiveconvolution.hxx}\\
    Namespace: vigra
    
    \begin{verbatim}
    vigra::FImage src(w,h), dest(w,h);    
    ...
    
    vigra::recursiveFirstDerivativeY(srcImageRange(src), destImage(dest), 3.0);
    
    \end{verbatim}

    @memo
*/
template <class SrcImageIterator, class SrcAccessor,
          class DestImageIterator, class DestAccessor>
void recursiveFirstDerivativeY(SrcImageIterator supperleft, 
                      SrcImageIterator slowerright, SrcAccessor as,
                      DestImageIterator dupperleft, DestAccessor ad, 
              double scale)
{
    int w = slowerright.x - supperleft.x;
    int h = slowerright.y - supperleft.y;
    
    int x;
    
    for(x=0; x<w; ++x, ++supperleft.x, ++dupperleft.x)
    {
        ColumnIterator<SrcImageIterator> cs(supperleft);
        ColumnIterator<DestImageIterator> cd(dupperleft);

        recursiveFirstDerivativeLine(cs, cs+h, accessorAdapter(cs, as), 
                                     cd, accessorAdapter(cd, ad), 
                                     scale);
    }
}
            
template <class SrcImageIterator, class SrcAccessor,
          class DestImageIterator, class DestAccessor>
inline void recursiveFirstDerivativeY(
            triple<SrcImageIterator, SrcImageIterator, SrcAccessor> src,
            pair<DestImageIterator, DestAccessor> dest, 
        double scale)
{
    recursiveFirstDerivativeY(src.first, src.second, src.third,
                          dest. first, dest.second, scale);
}
            
/********************************************************/
/*                                                      */
/*             recursiveSecondDerivativeX               */
/*                                                      */
/********************************************************/

/** Recursively calculates the 1 dimensional second derivative in x 
    direction by calling \Ref{recursiveSecondDerivativeLine}() for every 
    row of the image. See \Ref{recursiveSecondDerivativeLine}() for more 
    information about required interfaces and vigra_preconditions.
    
    {\bf Declarations:}
    
    pass arguments explicitly:
    \begin{verbatim}
    namespace vigra {
        template <class SrcImageIterator, class SrcAccessor,
              class DestImageIterator, class DestAccessor>
        void recursiveSecondDerivativeX(SrcImageIterator supperleft, 
                  SrcImageIterator slowerright, SrcAccessor as,
                  DestImageIterator dupperleft, DestAccessor ad, 
                  double scale)
    }
    \end{verbatim}
    
    
    use argument objects in conjuction with \Ref{Argument Object Factories}:
    \begin{verbatim}
    namespace vigra {
        template <class SrcImageIterator, class SrcAccessor,
              class DestImageIterator, class DestAccessor>
        void recursiveSecondDerivativeX(
            triple<SrcImageIterator, SrcImageIterator, SrcAccessor> src,
            pair<DestImageIterator, DestAccessor> dest, 
            double scale)
    }
    \end{verbatim}
    
    {\bf Usage:}
    
    Include-File:
    \URL[vigra/recursiveconvolution.hxx]{../include/vigra/recursiveconvolution.hxx}\\
    Namespace: vigra
    
    \begin{verbatim}
    vigra::FImage src(w,h), dest(w,h);    
    ...
    
    vigra::recursiveSecondDerivativeX(srcImageRange(src), destImage(dest), 3.0);
    
    \end{verbatim}

    @memo
*/
template <class SrcImageIterator, class SrcAccessor,
          class DestImageIterator, class DestAccessor>
void recursiveSecondDerivativeX(SrcImageIterator supperleft, 
                      SrcImageIterator slowerright, SrcAccessor as,
                      DestImageIterator dupperleft, DestAccessor ad, 
              double scale)
{
    int w = slowerright.x - supperleft.x;
    int h = slowerright.y - supperleft.y;
    
    int y;
    
    for(y=0; y<h; ++y, ++supperleft.y, ++dupperleft.y)
    {
        RowIterator<SrcImageIterator> rs(supperleft);
        RowIterator<DestImageIterator> rd(dupperleft);

        recursiveSecondDerivativeLine(rs, rs+w, accessorAdapter(rs, as), 
                                      rd, accessorAdapter(rd, ad), 
                                      scale);
    }
}
            
template <class SrcImageIterator, class SrcAccessor,
          class DestImageIterator, class DestAccessor>
inline void recursiveSecondDerivativeX(
            triple<SrcImageIterator, SrcImageIterator, SrcAccessor> src,
            pair<DestImageIterator, DestAccessor> dest, 
        double scale)
{
    recursiveSecondDerivativeX(src.first, src.second, src.third,
                          dest. first, dest.second, scale);
}
            
/********************************************************/
/*                                                      */
/*             recursiveSecondDerivativeY               */
/*                                                      */
/********************************************************/

/** Recursively calculates the 1 dimensional second derivative in y 
    direction by calling \Ref{recursiveSecondDerivativeLine}() for every 
    column of the image. See \Ref{recursiveSecondDerivativeLine}() for more 
    information about required interfaces and vigra_preconditions.
    
    {\bf Declarations:}
    
    pass arguments explicitly:
    \begin{verbatim}
    namespace vigra {
        template <class SrcImageIterator, class SrcAccessor,
              class DestImageIterator, class DestAccessor>
        void recursiveSecondDerivativeY(SrcImageIterator supperleft, 
                  SrcImageIterator slowerright, SrcAccessor as,
                  DestImageIterator dupperleft, DestAccessor ad, 
                  double scale)
    }
    \end{verbatim}
    
    
    use argument objects in conjuction with \Ref{Argument Object Factories}:
    \begin{verbatim}
    namespace vigra {
        template <class SrcImageIterator, class SrcAccessor,
              class DestImageIterator, class DestAccessor>
        void recursiveSecondDerivativeY(
            triple<SrcImageIterator, SrcImageIterator, SrcAccessor> src,
            pair<DestImageIterator, DestAccessor> dest, 
            double scale)
    }
    \end{verbatim}
    
    {\bf Usage:}
    
    Include-File:
    \URL[vigra/recursiveconvolution.hxx]{../include/vigra/recursiveconvolution.hxx}\\
    Namespace: vigra
    
    \begin{verbatim}
    vigra::FImage src(w,h), dest(w,h);    
    ...
    
    vigra::recursiveSecondDerivativeY(srcImageRange(src), destImage(dest), 3.0);
    
    \end{verbatim}

    @memo
*/
template <class SrcImageIterator, class SrcAccessor,
          class DestImageIterator, class DestAccessor>
void recursiveSecondDerivativeY(SrcImageIterator supperleft, 
                      SrcImageIterator slowerright, SrcAccessor as,
                      DestImageIterator dupperleft, DestAccessor ad, 
              double scale)
{
    int w = slowerright.x - supperleft.x;
    int h = slowerright.y - supperleft.y;
    
    int x;
    
    for(x=0; x<w; ++x, ++supperleft.x, ++dupperleft.x)
    {
        ColumnIterator<SrcImageIterator> cs(supperleft);
        ColumnIterator<DestImageIterator> cd(dupperleft);

        recursiveSecondDerivativeLine(cs, cs+h, accessorAdapter(cs, as), 
                                      cd, accessorAdapter(cd, ad), 
                                      scale);
    }
}
            
template <class SrcImageIterator, class SrcAccessor,
          class DestImageIterator, class DestAccessor>
inline void recursiveSecondDerivativeY(
            triple<SrcImageIterator, SrcImageIterator, SrcAccessor> src,
            pair<DestImageIterator, DestAccessor> dest, 
        double scale)
{
    recursiveSecondDerivativeY(src.first, src.second, src.third,
                          dest. first, dest.second, scale);
}
            
//@}

} // namespace vigra

#endif // VIGRA_RECURSIVECONVOLUTION_HXX
