qSim
====

A quantum computer simulator.

This was originally a project for CS505: Advanced Theory of Computation. Since
then, I have worked on it intermittently. For a simplified explanation of the
theory of quantum circuits and the quantum mechanics this simulator is supposed
to model, see the included paper.

Building
--------

Just `cd` into the project directory and run `make`. Should work out of the box.

Usage
-----

    qsim <flags> -f <qasm_file> <initial_register_value>

###### Flags
- `-f` Specify qasm file to read operations from. This is required for now.
  In the future, this will be optional in order to allow qSim to function as
  a repl.
- `-s` Single step mode. With this flag, the value of the register will be
  printed after each operation. Without it, only the result will be displayed.
- `-h` Print help.

qSim processes a simple language of quantum gates called *qasm*. Qasm files
should have the extension `.q` or `.Q`, although technically qSim doesn't care.
Each line but the first specifies a sequential quantum gate to apply to the
register of qubits. Blank lines are ignored.

Qasm provides a small but universal set of gates: `X`, `Y`, `Z`, `S`, `T`,
`hadamard`, and `cnot`. Any gate may optionally be followed by `*` to indicate
that the conjugate transpose of that gate should be used. The `cnot` gate
expects exactly two comma-separated operands, refering to the control and target
qubits, in that order. All other gates can take an arbitrary number of
operands, up to the width of the register, meaning that the operation should be
applied to the qubits in parallel. Bit positions are specified in big-endian
order, with an LSB index of 0.

Each file must begin with a line of the form `reg SIZE`, where `SIZE` is the
number of qubits in the single register, up to a max of 32. The initial value of
the register is then passed as a command-line argument to qSim in the form of
a decimal or hexidecimal (prefixed with `0x`) integer. If you need any scratch
qubits, they will need to be supplied in the same integer as the input. Register
values are in big-endian order.

The following is a (completely useless) example of a qasm file:

    reg 4
    h   0,1
    cx  1,2
    s*  0,2,3
    x   1

On a typical run, using `./qsim -sf test.q 0`, the output might look like this
(note that the result will differ from run to run, since it is the result of
collapsing the distribution introduced by the superposition of qubits):

    |psi_0> = (1) |0000>
    |psi_1> = (0.5) |0000> + (0.5) |0001> + (0.5) |0010> + (0.5) |0011>
    |psi_2> = (0.5) |0000> + (0.5) |0001> + (0.5) |0110> + (0.5) |0111>
    |psi_3> = (0.5) |0000> + (-0.5i) |0001> + (-0.5i) |0110> + (-0.5) |0111>
    |psi_4> = (0.5) |0010> + (-0.5i) |0011> + (-0.5i) |0100> + (-0.5) |0101>
    result: 0x3

Qasm allows the following variations are allowed for keywords:

    X|x|not
    Y|y
    Z|z
    H|h|hadamard
    S|s|phase
    T|t
    CX|CNOT|cx|cnot
    REGISTER|REG|register|reg

Known bugs
----------

These are qSim's known bad behaviors. There are many like them, but these ones
are qSim's:

- Specifying a register width of 0 or greater than 32 causes an assert failure,
  rather than producing a sane error message.
- Empty qasm files cause an assert failure, rather than a sane error message.
- Because of the exponential blowup in number of states, using register widths
  greater than about 9 tends to cause stack overflows.
- Specifying qubit positions outside the register silently and harmlessly fails,
  whereas it should probably cause a warning or an error.
