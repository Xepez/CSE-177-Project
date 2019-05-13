SELECT SUM(l_discount) 
FROM customer, orders, lineitem
WHERE c_custkey = o_custkey AND o_orderkey = l_orderkey AND
	c_name = 'Customer#000070919' AND l_quantity > 30.0 AND l_discount < 0.03

