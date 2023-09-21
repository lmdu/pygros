pygros
======

a python library for finding genomic range overlaps based on `cgranges <https://github.com/lh3/cgranges>`_.

Installation
------------

``pygros`` is available on `PyPi <https://pypi.org>`_, to install it:

.. code:: bash

	pip install pygros

Usage
-----

.. code:: python

	>>> import pygros
	>>> ranges = pygros.Ranges()
	>>> ranges.add('chr1', 10, 20)
	>>> ranges.add('chr1', 30, 50)
	>>> ranges.add('chr1', 25, 40)
	>>> ranges.index()
	>>> ranges.overlap('chr1', 30, 40)
	[(25, 40, -1), (30, 50, -1)]

API reference
-------------

pygros.Ranges(intervals=[])

	create a Ranges object to store genomic ranges

	@param intervals: a list or tuple containing multiple ranges

	@return Ranges object

	add(chrom, start, end, label=-1)

		add genomic range into Ranges object

		@param chrom: chromosome name or sequence name

		@param start: start of range

		@param end: end of range

		@param label: an integer

	index()

		After add new genomic ranges, use this method to build index

	overlap(chrom, start, end)

		get genomic ranges that overlapped with your given range (start, end)

		@param chrom: chromosome or sequence name

		@param start: start of range

		@param end: end of range

		@return: a list of ranges 

	within(chrom, start, end)

		get genomic ranges within given range (start, end)

		@param chrom: chromosome or sequence name

		@param start: start of range

		@param end: end of range

		@return: a list of ranges

	contain(chrom, start, end)

		get genomic ranges that contained in given range (start, end)

		@param chrom: chromosome or sequence name

		@param start: start of range

		@param end: end of range

		@return: a list of ranges
