% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/nanoduration.R
\name{seq.nanoduration}
\alias{seq.nanoduration}
\title{Sequence Generation}
\usage{
\method{seq}{nanoduration}(from, to = NULL, by = NULL, length.out = NULL, along.with = NULL, ...)
}
\arguments{
\item{from, to}{the starting and (maximal) end values of the
sequence}

\item{by}{the increment of the sequence}

\item{length.out}{integer indicating the desired length of the sequence}

\item{along.with}{take the length from the length of this argument.}

\item{...}{arguments passed to or from methods}
}
\description{
Generate a sequence of \code{nanoduration}
}
\examples{
seq(from=as.nanoduration(0), by=as.nanoduration("01:00:00"), length.out=10)
}
