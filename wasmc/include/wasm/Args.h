#pragma once

#include <memory>
#include <vector>

namespace Wasmc {
	class ASTNode;

	struct Arg {
		enum class Type {Register, Address, Value, Number};
		virtual ~Arg() {}
		virtual Arg * copy() const = 0;
		virtual Type getType() = 0;
		virtual operator std::string() const = 0;
	};

	struct RegisterArg: Arg {
		int reg;
		RegisterArg(ASTNode *);
		RegisterArg(int reg_): reg(reg_) {}
		Arg * copy() const override { return new RegisterArg(reg); }
		Type getType() override { return Type::Register; }
		operator std::string() const override;
	};

	struct AddressArg: Arg {
		const std::string *ident;
		AddressArg(ASTNode *);
		AddressArg(const std::string *ident_): ident(ident_) {}
		Arg * copy() const override { return new AddressArg(ident); }
		Type getType() override { return Type::Address; }
		operator std::string() const override;
	};

	struct ValueArg: Arg {
		const std::string *ident;
		ValueArg(ASTNode *);
		ValueArg(const std::string *ident_): ident(ident_) {}
		Arg * copy() const override { return new ValueArg(ident); }
		Type getType() override { return Type::Value; }
		operator std::string() const override;
	};

	struct NumberArg: Arg {
		int value;
		NumberArg(ASTNode *);
		NumberArg(int value_): value(value_) {}
		Arg * copy() const override { return new NumberArg(value); }
		Type getType() override { return Type::Number; }
		operator std::string() const override;
	};

	struct Args {
		std::vector<std::unique_ptr<Arg>> args;
		Args(ASTNode *);
		Args(const Args &);
		Arg & operator[](size_t index) const { return *args[index]; }
		size_t size() const { return args.size(); }
		bool empty() const { return args.empty(); }
		decltype(args)::const_iterator begin() const { return args.begin(); }
		decltype(args)::const_iterator end() const { return args.end(); }
		decltype(args)::const_iterator cbegin() const { return args.cbegin(); }
		decltype(args)::const_iterator cend() const { return args.cend(); }
	};
}
