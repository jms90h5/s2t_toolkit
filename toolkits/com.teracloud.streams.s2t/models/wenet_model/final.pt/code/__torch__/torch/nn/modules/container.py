class Sequential(Module):
  __parameters__ = []
  __buffers__ = []
  training : bool
  _is_full_backward_hook : Optional[bool]
  __annotations__["0"] = __torch__.torch.nn.modules.conv.Conv2d
  __annotations__["1"] = __torch__.torch.nn.modules.activation.ReLU
  __annotations__["2"] = __torch__.torch.nn.modules.conv.___torch_mangle_0.Conv2d
  __annotations__["3"] = __torch__.torch.nn.modules.activation.ReLU
  def forward(self: __torch__.torch.nn.modules.container.Sequential,
    input: Tensor) -> Tensor:
    _0 = getattr(self, "0")
    _1 = getattr(self, "1")
    _2 = getattr(self, "2")
    _3 = getattr(self, "3")
    input0 = (_0).forward(input, )
    input1 = (_1).forward(input0, )
    input2 = (_2).forward(input1, )
    return (_3).forward(input2, )
  def __len__(self: __torch__.torch.nn.modules.container.Sequential) -> int:
    return 4
class ModuleList(Module):
  __parameters__ = []
  __buffers__ = []
  training : bool
  _is_full_backward_hook : Optional[bool]
  __annotations__["0"] = __torch__.wenet.transformer.encoder_layer.ConformerEncoderLayer
  __annotations__["1"] = __torch__.wenet.transformer.encoder_layer.ConformerEncoderLayer
  __annotations__["2"] = __torch__.wenet.transformer.encoder_layer.ConformerEncoderLayer
  __annotations__["3"] = __torch__.wenet.transformer.encoder_layer.ConformerEncoderLayer
  __annotations__["4"] = __torch__.wenet.transformer.encoder_layer.ConformerEncoderLayer
  __annotations__["5"] = __torch__.wenet.transformer.encoder_layer.ConformerEncoderLayer
  __annotations__["6"] = __torch__.wenet.transformer.encoder_layer.ConformerEncoderLayer
  __annotations__["7"] = __torch__.wenet.transformer.encoder_layer.ConformerEncoderLayer
  __annotations__["8"] = __torch__.wenet.transformer.encoder_layer.ConformerEncoderLayer
  __annotations__["9"] = __torch__.wenet.transformer.encoder_layer.ConformerEncoderLayer
  __annotations__["10"] = __torch__.wenet.transformer.encoder_layer.ConformerEncoderLayer
  __annotations__["11"] = __torch__.wenet.transformer.encoder_layer.ConformerEncoderLayer
  def __len__(self: __torch__.torch.nn.modules.container.ModuleList) -> int:
    return 12
