---
description: 'The result is equal to the square root of varSamp. Unlike this function
  uses a numerically stable algorithm.'
sidebar_position: 191
slug: /sql-reference/aggregate-functions/reference/stddevsampstable
title: 'stddevSampStable'
---

# stddevSampStable

The result is equal to the square root of [varSamp](../../../sql-reference/aggregate-functions/reference/varsamp.md). Unlike [`stddevSamp`](../reference/stddevsamp.md) this function uses a numerically stable algorithm. It works slower but provides a lower computational error.

**Syntax**

```sql
stddevSampStable(x)
```

**Parameters**

- `x`: Values for which to find the square root of sample variance. [(U)Int*](../../data-types/int-uint.md), [Float*](../../data-types/float.md), [Decimal*](../../data-types/decimal.md).

**Returned value**

Square root of sample variance of `x`. [Float64](../../data-types/float.md).

**Example**

Query:

```sql
DROP TABLE IF EXISTS test_data;
CREATE TABLE test_data
(
    population UInt8,
)
ENGINE = Log;

INSERT INTO test_data VALUES (3),(3),(3),(4),(4),(5),(5),(7),(11),(15);

SELECT
    stddevSampStable(population)
FROM test_data;
```

Result:

```response
┌─stddevSampStable(population)─┐
│                            4 │
└──────────────────────────────┘
```