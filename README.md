
# brutemedia

## Description

specifications and reference implementation of brutelang.

it has both dynamic and static typing.(currently only dynamic specifications are described)

## Table of Contents

- [Types](#types)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Types


- `$variable` = args that starts with $, can be anything, variables are stored in "variables"

- `number` = args that starts with 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 or -

- `string` = any other args

## Usage

- argless functions:

`function;`


- returnless function:

`function arg1 arg2 ...;`


- function which return goes to $target_variable (":" denotes a return, the target variable must be on the left):

`target_variable:function ...;`


- argless function which return goes to $target_variable:

`target_variable:function;`


- variables always starts with $:

`function $variable_1 $variable_2 ...;`


## Contributing

[WORK IN PROGRESS]

## License

[WORK IN PROGRESS]