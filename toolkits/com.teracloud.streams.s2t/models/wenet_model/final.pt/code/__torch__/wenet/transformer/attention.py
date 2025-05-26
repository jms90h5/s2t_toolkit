class RelPositionMultiHeadedAttention(Module):
  __parameters__ = ["pos_bias_u", "pos_bias_v", ]
  __buffers__ = []
  pos_bias_u : Tensor
  pos_bias_v : Tensor
  training : bool
  _is_full_backward_hook : Optional[bool]
  d_k : int
  h : int
  linear_q : __torch__.torch.nn.quantized.dynamic.modules.linear.Linear
  linear_k : __torch__.torch.nn.quantized.dynamic.modules.linear.Linear
  linear_v : __torch__.torch.nn.quantized.dynamic.modules.linear.Linear
  linear_out : __torch__.torch.nn.quantized.dynamic.modules.linear.Linear
  dropout : __torch__.torch.nn.modules.dropout.___torch_mangle_1.Dropout
  linear_pos : __torch__.torch.nn.quantized.dynamic.modules.linear.Linear
  def forward(self: __torch__.wenet.transformer.attention.RelPositionMultiHeadedAttention,
    query: Tensor,
    key: Tensor,
    value: Tensor,
    mask: Tensor=CONSTANTS.c1,
    pos_emb: Tensor=CONSTANTS.c2,
    cache: Tensor=CONSTANTS.c0) -> Tuple[Tensor, Tensor]:
    _0 = (self).forward_qkv(query, key, value, )
    q, k, v, = _0
    q0 = torch.transpose(q, 1, 2)
    if torch.gt(torch.size(cache, 2), 0):
      _1 = torch.floordiv(torch.size(cache, -1), 2)
      key_cache, value_cache, = torch.split(cache, _1, -1)
      k0, v0 = torch.cat([key_cache, k], 2), torch.cat([value_cache, v], 2)
    else:
      k0, v0 = k, v
    new_cache = torch.cat([k0, v0], -1)
    n_batch_pos = torch.size(pos_emb, 0)
    _2 = (self.linear_pos).forward(pos_emb, )
    p = torch.view(_2, [n_batch_pos, -1, self.h, self.d_k])
    p0 = torch.transpose(p, 1, 2)
    q_with_bias_u = torch.transpose(torch.add(q0, self.pos_bias_u), 1, 2)
    q_with_bias_v = torch.transpose(torch.add(q0, self.pos_bias_v), 1, 2)
    matrix_ac = torch.matmul(q_with_bias_u, torch.transpose(k0, -2, -1))
    matrix_bd = torch.matmul(q_with_bias_v, torch.transpose(p0, -2, -1))
    scores = torch.div(torch.add(matrix_ac, matrix_bd), torch.sqrt(self.d_k))
    _3 = (self).forward_attention(v0, scores, mask, )
    return (_3, new_cache)
  def forward_qkv(self: __torch__.wenet.transformer.attention.RelPositionMultiHeadedAttention,
    query: Tensor,
    key: Tensor,
    value: Tensor) -> Tuple[Tensor, Tensor, Tensor]:
    n_batch = torch.size(query, 0)
    q = torch.view((self.linear_q).forward(query, ), [n_batch, -1, self.h, self.d_k])
    k = torch.view((self.linear_k).forward(key, ), [n_batch, -1, self.h, self.d_k])
    v = torch.view((self.linear_v).forward(value, ), [n_batch, -1, self.h, self.d_k])
    q1 = torch.transpose(q, 1, 2)
    k1 = torch.transpose(k, 1, 2)
    v1 = torch.transpose(v, 1, 2)
    return (q1, k1, v1)
  def forward_attention(self: __torch__.wenet.transformer.attention.RelPositionMultiHeadedAttention,
    value: Tensor,
    scores: Tensor,
    mask: Tensor=CONSTANTS.c1) -> Tensor:
    n_batch = torch.size(value, 0)
    if torch.gt(torch.size(mask, 2), 0):
      mask0 = torch.eq(torch.unsqueeze(mask, 1), 0)
      scores0 = torch.masked_fill(scores, mask0, -inf)
      attn0 = torch.masked_fill(torch.softmax(scores0, -1), mask0, 0.)
      attn = attn0
    else:
      attn = torch.softmax(scores, -1)
    p_attn = (self.dropout).forward(attn, )
    x = torch.matmul(p_attn, value)
    _4 = torch.contiguous(torch.transpose(x, 1, 2))
    _5 = [n_batch, -1, torch.mul(self.h, self.d_k)]
    x0 = torch.view(_4, _5)
    return (self.linear_out).forward(x0, )
class MultiHeadedAttention(Module):
  __parameters__ = []
  __buffers__ = []
  training : bool
  _is_full_backward_hook : Optional[bool]
  d_k : int
  h : int
  linear_q : __torch__.torch.nn.quantized.dynamic.modules.linear.Linear
  linear_k : __torch__.torch.nn.quantized.dynamic.modules.linear.Linear
  linear_v : __torch__.torch.nn.quantized.dynamic.modules.linear.Linear
  linear_out : __torch__.torch.nn.quantized.dynamic.modules.linear.Linear
  dropout : __torch__.torch.nn.modules.dropout.___torch_mangle_1.Dropout
  def forward(self: __torch__.wenet.transformer.attention.MultiHeadedAttention,
    query: Tensor,
    key: Tensor,
    value: Tensor,
    mask: Tensor=CONSTANTS.c1,
    pos_emb: Tensor=CONSTANTS.c2,
    cache: Tensor=CONSTANTS.c0) -> Tuple[Tensor, Tensor]:
    _6 = (self).forward_qkv(query, key, value, )
    q, k, v, = _6
    if torch.gt(torch.size(cache, 2), 0):
      _7 = torch.floordiv(torch.size(cache, -1), 2)
      key_cache, value_cache, = torch.split(cache, _7, -1)
      k2, v2 = torch.cat([key_cache, k], 2), torch.cat([value_cache, v], 2)
    else:
      k2, v2 = k, v
    new_cache = torch.cat([k2, v2], -1)
    _8 = torch.matmul(q, torch.transpose(k2, -2, -1))
    scores = torch.div(_8, torch.sqrt(self.d_k))
    _9 = (self).forward_attention(v2, scores, mask, )
    return (_9, new_cache)
  def forward_qkv(self: __torch__.wenet.transformer.attention.MultiHeadedAttention,
    query: Tensor,
    key: Tensor,
    value: Tensor) -> Tuple[Tensor, Tensor, Tensor]:
    n_batch = torch.size(query, 0)
    q = torch.view((self.linear_q).forward(query, ), [n_batch, -1, self.h, self.d_k])
    k = torch.view((self.linear_k).forward(key, ), [n_batch, -1, self.h, self.d_k])
    v = torch.view((self.linear_v).forward(value, ), [n_batch, -1, self.h, self.d_k])
    q2 = torch.transpose(q, 1, 2)
    k3 = torch.transpose(k, 1, 2)
    v3 = torch.transpose(v, 1, 2)
    return (q2, k3, v3)
  def forward_attention(self: __torch__.wenet.transformer.attention.MultiHeadedAttention,
    value: Tensor,
    scores: Tensor,
    mask: Tensor=CONSTANTS.c1) -> Tensor:
    n_batch = torch.size(value, 0)
    if torch.gt(torch.size(mask, 2), 0):
      mask1 = torch.eq(torch.unsqueeze(mask, 1), 0)
      scores1 = torch.masked_fill(scores, mask1, -inf)
      attn1 = torch.masked_fill(torch.softmax(scores1, -1), mask1, 0.)
      attn = attn1
    else:
      attn = torch.softmax(scores, -1)
    p_attn = (self.dropout).forward(attn, )
    x = torch.matmul(p_attn, value)
    _10 = torch.contiguous(torch.transpose(x, 1, 2))
    _11 = [n_batch, -1, torch.mul(self.h, self.d_k)]
    x1 = torch.view(_10, _11)
    return (self.linear_out).forward(x1, )
