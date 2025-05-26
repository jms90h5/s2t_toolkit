class RelPositionalEncoding(Module):
  __parameters__ = []
  __buffers__ = []
  training : bool
  _is_full_backward_hook : Optional[bool]
  d_model : int
  xscale : float
  max_len : int
  pe : Tensor
  dropout : __torch__.torch.nn.modules.dropout.Dropout
  def forward(self: __torch__.wenet.transformer.embedding.RelPositionalEncoding,
    x: Tensor,
    offset: int=0) -> Tuple[Tensor, Tensor]:
    _0 = torch.lt(torch.add(offset, torch.size(x, 1)), self.max_len)
    if _0:
      pass
    else:
      ops.prim.RaiseException("AssertionError: ")
    _1 = torch.to(self.pe, ops.prim.device(x))
    self.pe = _1
    x0 = torch.mul(x, self.xscale)
    _2 = torch.slice(self.pe)
    _3 = torch.add(offset, torch.size(x0, 1))
    pos_emb = torch.slice(_2, 1, offset, _3)
    _4 = ((self.dropout).forward(x0, ), (self.dropout).forward(pos_emb, ))
    return _4
  def position_encoding(self: __torch__.wenet.transformer.embedding.RelPositionalEncoding,
    offset: int,
    size: int) -> Tensor:
    _5 = torch.lt(torch.add(offset, size), self.max_len)
    if _5:
      pass
    else:
      ops.prim.RaiseException("AssertionError: ")
    _6 = self.dropout
    _7 = torch.slice(torch.slice(self.pe), 1, offset, torch.add(offset, size))
    return (_6).forward(_7, )
class PositionalEncoding(Module):
  __parameters__ = []
  __buffers__ = []
  training : bool
  _is_full_backward_hook : Optional[bool]
  d_model : int
  xscale : float
  max_len : int
  pe : Tensor
  dropout : __torch__.torch.nn.modules.dropout.Dropout
  def forward(self: __torch__.wenet.transformer.embedding.PositionalEncoding,
    x: Tensor,
    offset: int=0) -> Tuple[Tensor, Tensor]:
    _8 = torch.lt(torch.add(offset, torch.size(x, 1)), self.max_len)
    if _8:
      pass
    else:
      ops.prim.RaiseException("AssertionError: ")
    _9 = torch.to(self.pe, ops.prim.device(x))
    self.pe = _9
    pos_emb = torch.slice(torch.slice(self.pe), 1, offset, torch.add(offset, torch.size(x, 1)))
    x1 = torch.add(torch.mul(x, self.xscale), pos_emb)
    _10 = ((self.dropout).forward(x1, ), (self.dropout).forward(pos_emb, ))
    return _10
