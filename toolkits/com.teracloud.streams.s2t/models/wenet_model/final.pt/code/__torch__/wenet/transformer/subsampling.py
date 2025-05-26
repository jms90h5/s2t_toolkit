class Conv2dSubsampling6(Module):
  __parameters__ = []
  __buffers__ = []
  training : bool
  _is_full_backward_hook : Optional[bool]
  right_context : int
  subsampling_rate : int
  conv : __torch__.torch.nn.modules.container.Sequential
  linear : __torch__.torch.nn.quantized.dynamic.modules.linear.Linear
  pos_enc : __torch__.wenet.transformer.embedding.RelPositionalEncoding
  def forward(self: __torch__.wenet.transformer.subsampling.Conv2dSubsampling6,
    x: Tensor,
    x_mask: Tensor,
    offset: int=0) -> Tuple[Tensor, Tensor, Tensor]:
    x0 = torch.unsqueeze(x, 1)
    x1 = (self.conv).forward(x0, )
    b, c, t, f, = torch.size(x1)
    _0 = self.linear
    _1 = torch.contiguous(torch.transpose(x1, 1, 2))
    _2 = torch.view(_1, [b, t, torch.mul(c, f)])
    x2 = (_0).forward(_2, )
    _3 = (self.pos_enc).forward(x2, offset, )
    x3, pos_emb, = _3
    _4 = torch.slice(torch.slice(torch.slice(x_mask), 1), 2, None, -2, 2)
    _5 = torch.slice(torch.slice(torch.slice(_4), 1), 2, None, -4, 3)
    return (x3, pos_emb, _5)
  def position_encoding(self: __torch__.wenet.transformer.subsampling.Conv2dSubsampling6,
    offset: int,
    size: int) -> Tensor:
    _6 = (self.pos_enc).position_encoding(offset, size, )
    return _6
