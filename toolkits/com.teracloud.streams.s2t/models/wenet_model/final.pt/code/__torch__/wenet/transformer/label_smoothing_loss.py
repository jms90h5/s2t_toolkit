class LabelSmoothingLoss(Module):
  __parameters__ = []
  __buffers__ = []
  training : bool
  _is_full_backward_hook : Optional[bool]
  padding_idx : int
  confidence : float
  smoothing : float
  size : int
  normalize_length : bool
  criterion : __torch__.torch.nn.modules.loss.KLDivLoss
  def forward(self: __torch__.wenet.transformer.label_smoothing_loss.LabelSmoothingLoss,
    x: Tensor,
    target: Tensor) -> Tensor:
    _0 = torch.eq(torch.size(x, 2), self.size)
    if _0:
      pass
    else:
      ops.prim.RaiseException("AssertionError: ")
    batch_size = torch.size(x, 0)
    x0 = torch.view(x, [-1, self.size])
    target0 = torch.view(target, [-1])
    true_dist = torch.zeros_like(x0)
    _1 = torch.div(self.smoothing, torch.sub(self.size, 1))
    _2 = torch.fill_(true_dist, _1)
    ignore = torch.eq(target0, self.padding_idx)
    total = torch.sub(torch.len(target0), torch.item(torch.sum(ignore)))
    target1 = torch.masked_fill(target0, ignore, 0)
    _3 = torch.scatter_(true_dist, 1, torch.unsqueeze(target1, 1), self.confidence)
    kl = (self.criterion).forward(torch.log_softmax(x0, 1), true_dist, )
    if self.normalize_length:
      denom = total
    else:
      denom = batch_size
    _4 = torch.masked_fill(kl, torch.unsqueeze(ignore, 1), 0)
    return torch.div(torch.sum(_4), denom)
