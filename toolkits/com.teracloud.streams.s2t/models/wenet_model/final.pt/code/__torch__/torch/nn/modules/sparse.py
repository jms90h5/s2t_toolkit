class Embedding(Module):
  __parameters__ = ["weight", ]
  __buffers__ = []
  weight : Tensor
  training : bool
  _is_full_backward_hook : NoneType
  sparse : Final[bool] = False
  scale_grad_by_freq : Final[bool] = False
  embedding_dim : Final[int] = 512
  max_norm : Final[NoneType] = None
  num_embeddings : Final[int] = 4999
  padding_idx : Final[NoneType] = None
  norm_type : Final[float] = 2.
  def forward(self: __torch__.torch.nn.modules.sparse.Embedding,
    input: Tensor) -> Tensor:
    _0 = __torch__.torch.nn.functional.embedding
    _1 = _0(input, self.weight, None, None, 2., False, False, )
    return _1
