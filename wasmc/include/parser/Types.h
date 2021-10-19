#pragma once

#include <memory>
#include <vector>

namespace Wasmc {
	class ASTNode;

	struct Type {
		virtual ~Type() {}
		virtual std::unique_ptr<Type> copy() const = 0;
	};

	struct ArrayType: Type {
		long count;
		std::unique_ptr<Type> subtype;
		ArrayType(long, const Type *);
		ArrayType(long, std::unique_ptr<Type> &&);
		std::unique_ptr<Type> copy() const override;
	};

	struct StructType: Type {
		std::vector<std::unique_ptr<Type>> subtypes;
		StructType(std::vector<std::unique_ptr<Type>> &&);
		std::unique_ptr<Type> copy() const override;
	};

	struct IntType: Type {
		size_t width;
		IntType(size_t width_);
		std::unique_ptr<Type> copy() const override;
	};

	struct PointerType: Type {
		std::unique_ptr<Type> subtype;
		PointerType(const Type *);
		PointerType(std::unique_ptr<Type> &&);
		std::unique_ptr<Type> copy() const override;
	};

	struct FunctionType: Type {
		FunctionType();
		std::unique_ptr<Type> copy() const override;
	};

	std::unique_ptr<Type> getType(const ASTNode *);
}
