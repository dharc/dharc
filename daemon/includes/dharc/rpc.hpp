/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_RPC_H_
#define DHARC_RPC_H_

#include <iostream>

namespace dharc {
namespace rpc {

/**
 * Read a command and all of its arguments from the input stream.
 * Execute the correct handler for that command.
 * Write the result of the command to the output stream.
 * @param is Input stream containing RPC message.
 * @param os Output stream to be filled with result message.
 */
void process_msg(std::istream &is, std::ostream &os);

};  // namespace rpc
};  // namespace dharc

#endif  /* DHARC_RPC_H_ */

