/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_RPC_H_
#define DHARC_RPC_H_

#include <utility>
#include <tuple>

#include "dharc/rpc_packer.hpp"

using dharc::rpc::Packer;

namespace dharc {
namespace rpc {

template<typename Ret>
Ret unpack(std::istream &is) {
	Ret res = Packer<Ret>::unpack(is);
	if (is.peek() == ',') is.ignore();
	return res;
}

template<int...> struct seq {};
template<int N, int... S> struct gens : gens<N-1, N-1, S...> {};
template<int... S> struct gens<0, S...>{ typedef seq<S...> type; };

template<typename Ret, typename T, typename F, int... S>
Ret callFunc(seq<S...>, T &params, F f) {
	return f(std::get<S>(params)...);
}

template <typename Ret, typename... Args>
void execute(std::istream &is, std::ostream &os, Ret(*f)(Args ...args)) {
	std::tuple<typename std::decay<Args>::type...> params {
		unpack<typename std::decay<Args>::type>(is)... };
	 Packer<Ret>::pack(os,
		callFunc<Ret>(typename gens<sizeof...(Args)>::type(), params, f));
}

void process_msg(std::istream &is, std::ostream &os);

};  // namespace rpc
};  // namespace dharc

#endif  /* DHARC_RPC_H_ */

