#ifndef ORDER_PARAMS_HPP
#define ORDER_PARAMS_HPP

#include <cstdint>

struct OrderId
{
	uint64_t value;
	OrderId(uint64_t id) : value(id) {}
};

struct ClientId
{
	uint64_t value;
	ClientId(uint64_t id) : value(id) {}
};

struct ClientOrderId
{
	uint64_t value;
	ClientOrderId(uint64_t id) : value(id) {}
};

enum class OrderType
{
	Market = 1,
	Limit = 2,
	Stop = 3,
	StopLimit = 4,
	TrailingStop = 5
};

enum class OrderSide
{
	Buy = 1,
	Sell = 2
};

enum class OrderTimeInForce
{
	Day = 1,
	GoodTillCancelled = 2,
	ImmediateOrCancel = 3,
	FillOrKill = 4
};

enum class OrderStatus
{
	New = 1,
	PartiallyFilled = 2,
	Filled = 3,
	Canceled = 4,
	Rejected = 5,
	PendingCancel = 6,
	PendingNew = 7
};

enum class OrderCapacity
{
	Agency = 'A',
	Principal = 'P'
};

#endif // ORDER_PARAMS_HPP