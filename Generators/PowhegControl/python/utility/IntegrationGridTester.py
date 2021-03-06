# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl IntegrationGridTester
#  Helper class to calculate and output integration grid quality
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
import glob, os, re

class IntegrationGridTester(object):
  '''Calculation of integration grid quality'''

  ## Regex to extract floats :: from perldoc perlretut
  __re_match_floats = r'[+-]? *(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?'
  # [+-]? *           # first, match an optional sign (and space)
  # (?:               # then match integers or f.p. mantissas:
  #   \d+             # start out with a ...
  #   (?:
  #     \.\d*         # mantissa of the form a.b or a.
  #   )?              # ? takes care of integers of the form a
  #  |\.\d+           # mantissa of the form .b
  # )
  # (?:[eE][+-]?\d+)? # finally, optionally match an exponent


  @classmethod
  def output_results( cls, logger ) :
    ## Read in integration quality metrics from output files
    #  Allow any check to fail, since this output is not a critical part of generation
    inclusive_xs, inclusive_xs_error, negative_weights, total_weights, n_events, n_upper_bound_failures = 0, 0, 0, 0, 0, 0
    for file_name in glob.glob( 'pwg*stat*.dat' ) :
      if os.path.isfile( file_name ) :
        # Inclusive cross-section uncertainty
        with open( file_name, 'rb' ) as data_file :
          try :
            matched_lines = [ line.replace('+-','') for line in data_file if re.match(r'(.*)(total(.*)cross section|suppression factor)(.*)[0-9](.*)\+\-(.*)[0-9](.*)', line) ]
            if len(matched_lines) > 0 :
              inclusive_xs_values = map( float, re.findall( cls.__re_match_floats, matched_lines[0] ) )
              inclusive_xs += inclusive_xs_values[0]
              inclusive_xs_error += inclusive_xs_values[1]
          except : # catch all exceptions
            pass
        # Negative weight test
        with open( file_name, 'rb' ) as data_file :
          try :
            matched_lines = [ line.replace('+-','') for line in data_file if re.match(r'(.*)(btilde \|neg\.\| weights|btilde Total \(pos.-\|neg.\|\))(.*)[0-9](.*)\+\-(.*)[0-9](.*)', line) ]
            if len(matched_lines) > 0 :
              negative_weights += map( float, re.findall( cls.__re_match_floats, [line for line in matched_lines if 'btilde |neg.|' in line][0] ) )[0]
              total_weights += map( float, re.findall( cls.__re_match_floats, [line for line in matched_lines if 'btilde Total' in line][0] ) )[0]
          except : # catch all exceptions
            pass

    for file_name in glob.glob( 'pwgcounters*.dat' ) :
      if os.path.isfile( file_name ) :
        # Upper bound violations
        with open( file_name, 'rb' ) as data_file :
          try :
            matched_lines = [ line.replace('+-','') for line in data_file if re.match(r'(.*)(btilde event|remnant event|upper bound failure)(.*)[0-9](.*)', line) ]
            if len(matched_lines) > 0 :
              n_events += map( float, re.findall( cls.__re_match_floats, [line for line in matched_lines if 'event' in line][0] ) )[0]
              n_upper_bound_failures += sum( map( float, sum( [ re.findall( cls.__re_match_floats, line ) for line in matched_lines if 'upper bound failure' in line ], [] ) ) )
          except : # catch all exceptions
            pass

    # Calculate test statistics
    try :
      inclusive_xs_test = 100 * inclusive_xs_error / inclusive_xs
    except : # catch all exceptions
      inclusive_xs_test = float('nan')
    try :
      negative_weight_test = 100 * negative_weights / total_weights
    except : # catch all exceptions
      negative_weight_test = float('nan')
    try :
      upper_bound_test = 100 * n_upper_bound_failures / n_events
    except : # catch all exceptions
      upper_bound_test = float('nan')

    # Write output
    try :
      getattr( logger, ['warning','info'][0.0 <= inclusive_xs_test < 1.0] )( 'Integration test :: {0:>25} : {1:.2f}%'.format('cross-section uncertainty', inclusive_xs_test) )
      getattr( logger, ['warning','info'][0.0 <= negative_weight_test < 1.0] )( 'Integration test :: {0:>25} : {1:.2f}%'.format('negative weight fraction', negative_weight_test) )
      getattr( logger, ['warning','info'][0.0 <= upper_bound_test < 1.0] )( 'Integration test :: {0:>25} : {1:.2f}%'.format('upper bound violations', upper_bound_test) )
    except : # catch all exceptions
      pass
